set(PROJECT_NAME "JGuardianHUB_SP")

idf_component_register(SRCS "JGuardianHUB.c;http_server.c;utility.c;ethernet_init.c"
                        INCLUDE_DIRS ".")
