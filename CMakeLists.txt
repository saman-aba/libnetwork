
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library( network STATIC
	src/net.c
	src/udp.c
	src/ether.c
	src/ip.c
	src/sctp.c
	src/if_info.c
)

target_include_directories( network PUBLIC
	src
)

add_library( diameter STATIC
	src/protocol/diameter/diameter.c
	src/protocol/diameter/diameter_dict.c
	src/val_str.c
)

target_include_directories( diameter PUBLIC
	src/protocol/diameter
)

target_link_libraries( diameter PRIVATE
	jansson
)
