# Referencia automatica dos backports de seguranca

O campo em Configuracoes > Sobre o telefone > Versao do Android continua separado dos niveis oficiais Android e fabricante.

A data fixa do overlay foi removida. build-quark.sh executa o verificador antes de carregar as regras da build e gera um overlay ignorado pelo Git em overlay-security-generated/<variante>/. Ele exporta QUARK_SECURITY_BACKPORT_OVERLAY somente para essa compilacao. Fora do wrapper, a referencia padrao fica vazia (Nao verificado).

O mes e o ultimo lote publicado verificavel na base local. A checagem usa os topicos R_asb_* para lineage-18.1 ou S_asb_* para lineage-19.1, desde agosto de 2025. Ela confere os lotes anteriores publicados no intervalo, objetos e ancestralidade Git, estado local e reversoes padrao. Propostas abertas nao avancam a referencia. Reversoes publicadas nos proprios lotes sao tratadas como transicoes esperadas do upstream.

Falha de rede ou auditoria incompleta gera referencia vazia. Nao se reutiliza cache de uma consulta anterior. A build pode prosseguir exibindo Nao verificado; falha ao gravar o overlay/relatorio impede prosseguir com um valor antigo.

Relatorio: OUT_DIR/quark-security-backports.json. Nenhum Git fetch, cherry-pick, sync ou alteracao de arquivo rastreado e realizado pelo verificador. Projetos declarados ausentes e historicos rasos sem os objetos exigidos nao recebem credito. Projetos fora do manifesto sao registrados fora do escopo.

Isso nao certifica cobertura completa de boletins/CVEs, nao verifica equivalencia funcional de codigo e nao detecta toda forma de desfazer uma correcao. Cherry-picks equivalentes que nao compartilhem o commit esperado exigem revisao. O arquivo lineage-backports.json e o registro historico da auditoria manual inicial, nao a fonte da data atual.
