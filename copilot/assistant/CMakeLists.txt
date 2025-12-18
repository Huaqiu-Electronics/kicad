add_library(assistant SHARED

    api/upload_project.cpp
    auth/dialog_login_base.cpp
    auth/dialog_login_webview.cpp

    common/copilot_dialog.cpp
    common/dialog_image_viewer.cpp
    common/dialog_webview.cpp
    common/main_stack_holder.cpp

    context/copilot_project_holder.cpp

    webview_library/dialog_add_component.cpp
    webview_library/dialog_add_design_block.cpp
    webview_library/panel_add_component.cpp
    webview_library/panel_add_design_block.cpp

    http/http_client.cpp

    mcp/mcp_client_event.cpp
    mcp/mcp_client_worker.cpp

    quote/pcb_quote.cpp
    quote/smt_quote.cpp
    quote/quote_controller.cpp

    sdk/kicad_sdk_http_server.cpp

    settings/site_env_mgr.cpp
    settings/webview_settings_manager.cpp

    utils/copilot_zip_utils.cpp
    utils/file_path_generator.cpp

    webview/internal/close_mcp_settings_dialog.cpp
    webview/internal/webview_controller.cpp
    webview/internal/webview_evt_filter.cpp

    webview/copilot_setup_service.cpp
    webview/copilot_webview_container.cpp
    webview/webview_container_script_msg_handle.cpp
    webview/webview_container.cpp
)

set_target_properties(assistant PROPERTIES CXX_VISIBILITY_PRESET hidden)

target_include_directories(assistant
    PUBLIC
    .
    $<TARGET_PROPERTY:magic_enum,INTERFACE_INCLUDE_DIRECTORIES>
)

add_subdirectory(third_party)
find_package(OpenSSL REQUIRED)
find_package(nng CONFIG REQUIRED)

target_link_libraries(assistant PUBLIC
    nlohmann_json
    fmt::fmt
    OpenSSL::SSL
    OpenSSL::Crypto
    minizip    
    ${wxWidgets_LIBRARIES}
    nng::nng
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
