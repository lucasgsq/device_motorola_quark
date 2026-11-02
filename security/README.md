# Referencia de backports de seguranca

O campo em Configuracoes > Sobre o telefone > Versao do Android mostra o mes dos lotes de backports verificados na base. Nao modifica os niveis oficiais de seguranca Android ou do fabricante, nao certifica cobertura completa de CVEs e nao inclui automaticamente firmware ou kernel.

A referencia e definida no recurso config_lineageSecurityBackportMonth do overlay de Settings. Vazio significa Nao verificado. O mes nao e derivado da data da build ou do repo sync.

A11+KSU: 2026-10, baseado na conferencia de ancestralidade de 79 alteracoes do topico R_asb_2026-09 e 5 do R_asb_2026-10. A coleta corresponde as fontes locais, sem afirmar o conteudo de uma ROM anterior instalada. A presenca no historico nao substitui teste de seguranca nem verifica reversoes ou alteracoes equivalentes de todo o codigo.

A12+: sem mes confirmado. Deve permanecer vazio ate auditoria propria do lineage-19.1.

Ao atualizar o mes, repetir a conferencia das fontes e atualizar security/lineage-backports.json. Se trocar por uma base sem a auditoria correspondente, limpar o mes. Validar o campo apos compilar Settings e instalar a ROM completa.
