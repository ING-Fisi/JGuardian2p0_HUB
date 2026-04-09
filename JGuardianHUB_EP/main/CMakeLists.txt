set(PROJECT_NAME "JGuardianHUB_EP")

idf_component_register(SRCS "JGuardianHUB.c;ModBUS.c;http_server.c;utility.c;ethernet_init.c;ledstrip.c;ftp.c;Logging.c"
                        INCLUDE_DIRS ".")
