CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

SET ( OCLFILES
	monalisa.cl
)

# Copy the config files to the build tree for easier testing
FILE ( COPY ${OCLFILES} DESTINATION . )

INSTALL ( FILES ${OCLFILES} DESTINATION ${INSTALL_PREFIX_DATA}/examples/${EXECUTABLENAME}/code )
