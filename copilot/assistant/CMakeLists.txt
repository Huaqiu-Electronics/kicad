add_library(assistant SHARED
    auth/dialog_login.cpp
    common/dialog_image_viewer.cpp
    common/dialog_webview.cpp
    settings/get_webview_chat_path.cpp
    settings/webview_settings_manager.cpp
    webview/webview_container.cpp
    webview/copilot_webview_container.cpp
    webview/webview_evt_filter.cpp
    http/http_client.cpp
    quote/quote_controller.cpp
)

set_target_properties(assistant PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_include_directories(assistant
    PUBLIC
    .
    $<TARGET_PROPERTY:magic_enum,INTERFACE_INCLUDE_DIRECTORIES>
)


find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)


target_link_libraries(assistant PUBLIC
    nlohmann_json
    fmt::fmt
    OpenSSL::SSL 
    OpenSSL::Crypto
    ZLIB::ZLIB    
    ${wxWidgets_LIBRARIES}
)

target_compile_definitions(assistant PRIVATE COPILOT_SHARED_LIBRARY)

install(TARGETS assistant
    RUNTIME DESTINATION ${KICAD_LIB}
    LIBRARY DESTINATION ${KICAD_LIB}
    COMPONENT binary
)

if(APPLE)
    # puts library into the main kicad.app bundle in build tree
    set_target_properties(assistant PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${OSX_BUNDLE_BUILD_LIB_DIR}"
        INSTALL_NAME_DIR "${OSX_BUNDLE_BUILD_LIB_DIR}"
    )
    set_target_properties(assistant PROPERTIES INSTALL_RPATH
        "@executable_path/../Frameworks")
    set_target_properties(assistant PROPERTIES BUILD_WITH_INSTALL_RPATH 1)
endif()
