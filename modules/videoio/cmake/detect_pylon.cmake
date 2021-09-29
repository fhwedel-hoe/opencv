# --- Basler pylon SDK ---

if(NOT HAVE_PYLON)
  if(Pylon_FOUND)
    set(HAVE_PYLON TRUE)
    # TODO: Include libraries properly
    ocv_add_external_target(pylon "" "-L/opt/pylon/lib -l${Pylon_LIBRARIES}" "HAVE_PYLON")
  endif()
endif()

if(NOT HAVE_PYLON)
  message(STATUS "Could NOT find Basler pylon SDK.")
endif()

set(HAVE_PYLON ${HAVE_PYLON} PARENT_SCOPE)
