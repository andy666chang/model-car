
target_sources(app PRIVATE button/service.c)
target_sources(app PRIVATE throttle/service.c)

FILE(GLOB sh_src shell/*.c)
target_sources(app PRIVATE ${sh_src})
