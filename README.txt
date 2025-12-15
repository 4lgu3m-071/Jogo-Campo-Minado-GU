README – Campo Minado em C com Raylib
Este projeto é um jogo de Campo Minado desenvolvido em linguagem C utilizando a biblioteca Raylib. O objetivo principal do projeto é estudo e prática de programação, lógica, estruturas de dados, manipulação de arquivos, gráficos e áudio.

PERMISSÃO DE USO
Você tem total permissão para usar, estudar, modificar e redistribuir este código, inclusive para fins acadêmicos ou pessoais. O uso comercial também é permitido. Peço apenas que, caso utilize este código ou partes dele em outros projetos, seja dado o devido crédito ao autor original.
Autor: Marcelo Líryo
Não há qualquer garantia. O software é fornecido “como está”, sem responsabilidade do autor por eventuais problemas.

SOBRE O JOGO
O jogo possui sistema de nome do jogador, menu com seleção de dificuldade, Campo Minado clássico, modo Boss com expansão do tabuleiro e mecânicas diferentes, sistema de salvamento e carregamento em arquivo binário, cronômetro, sistema de bandeiras, música de fundo com controle de áudio e interface gráfica feita com Raylib.

COMO EXECUTAR
Pré-requisitos: Raylib instalada corretamente na máquina, compilador C atualizado (recomendado GCC mais recente) e sistema compatível com Raylib.

Compilação (exemplo usando GCC):
gcc jogo2.c -o jogo2 -lraylib -lopengl32 -lgdi32 -lwinmm
Observação: Os parâmetros de linkagem podem variar de acordo com o sistema operacional. Consulte a documentação oficial da Raylib se necessário.
Observação 2: o projeto já possui uma configuração de compilação pré-definida para o VS Code, através dos arquivos c_cpp_properties.json e tasks.json, facilitando a compilação e execução do código automaticamente.

Execução:
No terminal, navegue até a pasta do projeto e execute:
.\jogo
ou no Windows execute diretamente:
jogo.exe
Certifique-se de que os arquivos de fonte (.ttf) e áudio (.mp3) estejam na mesma pasta do executável.

CONSIDERAÇÕES FINAIS
Este projeto foi meu *magnum opus* do ano... Então, ao Prof. Dr. José Grimaldo da Silva Filho que estará checando o código, saiba que fiz grande esforço e tive muito carinho por ele.
