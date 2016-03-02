cmake_minimum_required(2.6)
project(PMB)

# Questa build è gestita da source e dalle sue sottocartelle
subdirs(source)

# Quì però è un bel posto dove mettere il comando per i file
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

# Copia-incollata da internet e leggermente modificata
if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(doxyfile ${PMB_SOURCE_DIR}/doxygen.conf)

	
	file(GLOB_RECURSE all_source_files "*.cpp" "*.hpp" "*.h" "*.cxx" "*.hxx")
	
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
		DEPENDS ${all_source_files}
        WORKING_DIRECTORY ${PMB_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
		)

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)
endif()