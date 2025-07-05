set(PROJECT_NAME "JGuardianHUB_EP")

idf_component_register(SRCS "JGuardianHUB.c;ModBUS.c;http_server.c;static_ip_connection.c;utility.c"
                        INCLUDE_DIRS ".")
