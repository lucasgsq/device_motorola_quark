# Inicializacao automatica ao conectar USB

Opcao em Configuracoes > Sistema > painel de acoes/gestos do Quark > Inicializacao. Desativada quando o arquivo de preferencia nao existe. Somente o usuario principal pode altera-la.

A preferencia fica exclusivamente em /persist/quark_usb_boot/enabled (0 ou 1, modo 0600, UID system), com escrita atomica. Nao e um dado de aplicativo, nao depende de /data descriptografado e permanece ate ser desativada, inclusive ao apagar apenas /data. Uma restauracao de backup do aplicativo nao ativa esse recurso em outro aparelho.

No modo charger, init monta uma visao privada de persist somente leitura, com noload, em /dev/quark-usb-pref, executa um leitor sem root e desmonta a visao. A montagem normal /persist nao e alterada. Falha, arquivo ausente ou formato invalido resultam em modo desativado.

Se ativada, o monitor espera tres leituras validas consecutivas, separadas por dois segundos: bateria presente, 5 a 100%, 3.6 a 4.5 V, saude Good, estado Charging/Full e USB presente/online. Se houver erro ou desconexao, a contagem reinicia. Esses limites reduzem o risco de boot com bateria descarregada, sem garantir estabilidade de uma bateria danificada. Nao se alteram limites de carregamento ou protecoes termicas.

A transicao usa sys.boot_from_charger_mode=1, ja suportada pelo init. Nao reinicia o aparelho nem modifica bootloader. Somente na transicao solicitada pelo helper (sys.quark.usb_autoboot_active=1), os limites de CPU impostos pelo modo charger sao restaurados para cpuinfo_max_freq, sem overclock. O ajuste normal de GPU permanece no fluxo boot_completed-power existente.

O helper usa o dominio charger e arquivo executavel charger_exec; o aplicativo system_app so acessa o diretorio exclusivo. A propriedade volatil sys.quark.usb_autoboot_enabled e de sistema, declarada no fork de system/sepolicy. Nao requer KernelSU, permissive, SELINUX_IGNORE_NEVERALLOWS ou shell root.

Validar por build completa: congelamento de API e neverallows, comportamento padrao desligado, gravacao da chave, boot automatico com USB, desligar a opcao novamente, WiFi/camera/audio e frequencias normais. Esta etapa nao foi testada desligando/reiniciando o aparelho via ADB: a autorizacao de coleta foi somente leitura. Um boot.img isolado nao instala o aplicativo, helper, init e politica.

A transicao padrao/manual, com o marcador automatico desligado, nao restaura frequencias por estas novas regras. O marcador e zerado ao entrar no modo charger e so e ativado apos todas as condicoes de boot automatico.
