FILE(GLOB userpics *.png *.jpg)
install(FILES ${userpics} DESTINATION ${DATA_INSTALL_DIR}/kdm/pics/users)
