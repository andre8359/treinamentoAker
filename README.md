

Este programa deverá ser operado por linha de comando e compilado sobre Linux. Ele deverá receber como parâmetro uma URI completa e um nome de arquivo, e em seguida conectar-se ao servidor, recuperar a página e salvá-la no arquivo informado. Não é permitido usar libfetch ou outras bibliotecas equivalentes. Desta forma, o novo funcionário deve implementar o protocolo HTTP (versão 1.0) para obter os dados remotos. Informações a respeito do protocolo podem ser obtidas aqui.

Este programa deve se preocupar especialmente com as seguintes condições de erro e avisá-las ao usuário corretamente:

    1 - Linha de comando incompleta ou com parâmetros mal formados;
    2 - Diferentes erros de rede (servidor não existe, não responde, etc);
    3 - Diferentes erros de arquivo (arquivo já existe, não pode ser acessado, etc).

Especificamente, no caso do arquivo informado já existir, deve haver uma flag na linha de comando que force sua sobrescrita.

Antes de concluída esta tarefa, a eficiência do recuperador deve ser avaliada e comparada com ferramentas de download equivalentes (por exemplo, wget).

O tempo esperado para o término desse exercício é de 20h de trabalho. Além do tratamento das condições de erro anteriores, é importante notar que as seguintes habilidades também serão avaliadas no exercício:

    1 - Claridade na leitura do código fonte, o que inclui nomenclatura de variáveis, disposição dos elementos sintáticos, existência e qualidade de comentários, entre outros;
    2 - Aderência completa às normas do protocolo tratado nesse exercício, o protocolo HTTP.

