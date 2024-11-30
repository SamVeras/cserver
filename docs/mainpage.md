@mainpage

# Main Page

## Introdução

Bem-vindo à documentação do projeto **cserver**.

A documentação está organizada para facilitar a navegação pelos principais
componentes do projeto, divididos entre cabeçalhos na pasta `include/` e
implementações na pasta `source/`.

## Estrutura do Projeto

### Pastas Principais:

- **docs/**: Contém a documentação do projeto.
- **include/**: Arquivos de cabeçalho (.h) com definições e interfaces públicas.
- **source/**: Implementações dos módulos (.c) do servidor.

### Principais Arquivos:

- `config.h` / `config.c`: Gerenciamento e leitura de configurações do servidor.
- `logging.h` / `logging.c`: Implementação de logs para depuração e monitoramento.
- `net_utils.h` / `net_utils.c`: Funções auxiliares e utilidades.
- `server.h` / `server.c`: Funções principais do servidor e sua inicialização.
- `sig.h` / `sig.c`: Gerenciamento de sinais do sistema

## Funcionalidades

- **Configuração por linha de comando**: Manipulação de configurações do servidor através de parâmetros.
- **Logging**: Registro detalhado de eventos e erros em arquivos ou console.
- **Conexões de Rede**: Manipulação de sockets e comunicação cliente-servidor.

---

_Gerado com Doxygen._
