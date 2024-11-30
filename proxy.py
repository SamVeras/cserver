import http.server
import requests
import urllib.parse
import os
import time
import hashlib
import certifi
from datetime import datetime
from urllib3 import disable_warnings

# eu não consegui ativar os certificados, mas como o servidor de proxy não vai
# ser usado pra nada sério, a gente só desativa os aviso mesmo
disable_warnings()


class ProxyCache:
    """
    Classe para gerenciar o cache de páginas HTML.

    Atributos:
    - cache_dir: o caminho para o diretório onde as páginas serão cacheadas.
    """

    def __init__(self, cache_dir="cache"):
        """
        Inicializa a classe com o diretório de cache especificado.

        Argumentos:
        - cache_dir: o caminho para o diretório onde as páginas serão cacheadas. Padrão: "cache".
        """
        self.cache_dir = cache_dir
        if not os.path.exists(cache_dir):
            os.makedirs(cache_dir)  # criar a pasta, caso já não exista

    def get_cache_path(self, url):
        """
        Retorna o caminho para o arquivo de cache da página com a URL especificada.

        Argumentos:
        - url: a URL da página que será cacheada.

        Retorna:
        - o caminho para o arquivo de cache da página.
        """
        # cria um nome de arquivo unico para a URL, baseado no seu hash MD5
        filename = hashlib.md5(url.encode()).hexdigest()
        return os.path.join(self.cache_dir, filename)  # concatena o caminho e o nome do arquivo

    def get_cached_content(self, url):
        """
        Retorna o conteudo da página cacheada com a URL especificada.

        Argumentos:
        - url: a URL da página que será cacheada.

        Retorna:
        - o conteudo da página cacheada, ou None se a página não estiver cacheada ainda.
        """
        cache_path = self.get_cache_path(url)
        if not os.path.exists(cache_path):
            return None

        if time.time() - os.path.getmtime(cache_path) > 300:
            # remover arquivos que não foram modificados por mais de 5 minutos
            os.remove(cache_path)
            return None

        with open(cache_path, "rb") as f:
            return f.read()

    def cache_content(self, url, content):
        """
        Cacheia o conteudo da página com a URL especificada.

        Argumentos:
        - url: a URL da página que será cacheada.
        - content: o conteudo da página que será cacheado.
        """
        cache_path = self.get_cache_path(url)
        with open(cache_path, "wb") as f:  # "wb" para escrever em binario
            f.write(content)


class ProxyHandler(http.server.BaseHTTPRequestHandler):
    """
    Classe que define o manipulador de requisições para o servidor proxy.

    Atributos:
    - cache: a instância da classe ProxyCache responsável pelo cache de páginas.
    """

    cache = ProxyCache()

    def resolve_url(self, url, base_url=None):
        """
        Resolve a URL relativa em uma URL absoluta.

        Argumentos:
        - url: a URL relativa a ser resolvida.
        - base_url: a URL base a ser usada para resolver a URL relativa.

        Retorna:
        - a URL absoluta resolvida.
        """
        if not url:
            return url

        # parsear a url base
        base_parts = urllib.parse.urlparse(base_url) if base_url else None

        # lidar com URLs relativas ao protocolo
        if url.startswith("//"):
            scheme = base_parts.scheme if base_parts else "https"
            return f"{scheme}:{url}"

        # lidar com URLs absolutas
        if url.startswith(("http://", "https://")):
            return url

        # lidar com URLs relativas à raiz
        if url.startswith("/"):
            if base_parts:
                return f"{base_parts.scheme}://{base_parts.netloc}{url}"
            return url

        # lidar com URLs relativas
        if base_parts:
            # remover qlqr parâmetros de consulta ou fragmentos da URL base antes de juntar
            base_path = base_parts.path
            if not base_path.endswith("/"):
                base_path = os.path.dirname(base_path) + "/"

            # construir a URL completa
            return urllib.parse.urljoin(
                f"{base_parts.scheme}://{base_parts.netloc}{base_path}", url
            )

        return url

    def modify_content(self, content, base_url):
        """
        Modifica o conteudo da página para incluir URLs resolvidas.

        Argumentos:
        - content: o conteudo da página a ser modificado.
        - base_url: a URL base a ser usada para resolver as URLs relativas.

        Retorna:
        - o conteudo da página modificado.
        """
        try:
            # decodificar o conteudo como UTF-8
            content_str = content.decode("utf-8")

            # encontrar e substituir URLs nos atributos src e href
            for attr in ["src=", "href="]:
                pos = 0
                while True:
                    pos = content_str.find(attr, pos)
                    if pos == -1:
                        break

                    # encontrar o caractere de aspas que fecha o valor do atributo
                    quote = content_str[pos + len(attr)]
                    if quote not in ['"', "'"]:
                        pos += len(attr)
                        continue

                    # encontrar o fim do valor do atributo
                    end_pos = content_str.find(quote, pos + len(attr) + 1)
                    if end_pos == -1:
                        break

                    # extrair a URL do valor do atributo
                    url = content_str[pos + len(attr) + 1 : end_pos]
                    if url and not url.startswith("#"):  # ignorar âncoras
                        # resolver a URL relativa
                        resolved_url = self.resolve_url(url, base_url)
                        if resolved_url != url:
                            # substituir a URL original com a URL resolvida
                            proxy_url = "/" + resolved_url
                            content_str = (
                                content_str[: pos + len(attr) + 1]
                                + proxy_url
                                + content_str[end_pos:]
                            )
                            # atualizar o pos pra continuar a busca a partir do final da URL substituída
                            pos = end_pos + len(proxy_url) - len(url)
                        else:
                            pos = end_pos
                    else:
                        pos = end_pos

            return content_str.encode("utf-8")
        except UnicodeDecodeError:
            # se o conteudo n pode ser decodificado como UTF-8, a gente só retorna o conteudo original
            return content

    def do_GET(self):
        """
        Manipula as requisições GET para o servidor proxy.
        """
        try:
            # parsear a url do request
            if self.path.startswith("/"):
                url = self.path[1:]
            else:
                url = self.path

            if not url.startswith(("http://", "https://")):
                url = "https://" + url  # HTTPS por padrão

            # verificar o cache primeiro
            cached_content = self.cache.get_cached_content(url)
            if cached_content:
                print(f"Servindo do cache: {url}")
                self.send_response(200)
                self.end_headers()
                self.wfile.write(cached_content)
                return

            # fazer a requisição com os headers corretos
            headers = {
                "Accept-Encoding": "identity",
                "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36",
            }

            print(f"Buscando: {url}")
            response = requests.get(url, headers=headers, verify=False)

            # enviar status da resposta
            self.send_response(response.status_code)

            # obter conteúdo e modificar se for HTML
            content = response.content
            content_type = response.headers.get("content-type", "")
            if "text/html" in content_type:
                content = self.modify_content(content, url)

            # enviar headers
            for key, value in response.headers.items():
                if key.lower() not in [
                    "transfer-encoding",
                    "connection",
                    "content-encoding",
                    "content-length",
                ]:
                    self.send_header(key, value)

            # definir o comprimento do conteúdo
            self.send_header("Content-Length", str(len(content)))
            self.end_headers()

            # cachear e enviar
            if response.status_code == 200:
                self.cache.cache_content(url, content)

            self.wfile.write(content)

        except Exception as e:
            print(f"Erro lidando com requisição: {e}")
            self.send_error(500, f"Erro lidando com requisição: {str(e)}")

    def log_message(self, format, *args):
        """
        Modifica a forma como as mensagens de log sãoo impressas.

        Argumentoss:
        - format: o formato da mensagem de log.
        - *args: os argumentos da mensagem de log.
        """
        print(f"[{datetime.now()}] {format%args}")


def run_proxy(host="localhost", port=8888):
    """
    Inicia o servidor proxy.

    Argumentos:
    - host: o nome do host ou o endereço IP onde o servidor proxy será executado.
    - port: a porta onde o servidor proxy será executado.
    """
    server = http.server.HTTPServer((host, port), ProxyHandler)
    print(f"Servidor de proxy iniciado em {host}:{port}")
    print(f"Exemplo: tente acessar http://{host}:{port}/www.example.com")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nFechando servidor de proxy...")
        server.shutdown()


if __name__ == "__main__":
    run_proxy()
