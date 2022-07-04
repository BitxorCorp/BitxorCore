cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME bitxorcore.mongo)

bitxorcore_library_target(${TARGET_NAME} mappers storages)
bitxorcore_add_mongo_dependencies(${TARGET_NAME})
# DO NOT alter the order, with model before plugins, linker is not pulling model::CreateNotificationPublisher producing invalid libextension.mongo as a result
# bitxorcore.validators is needed due to validators::AggregateValidationResult
target_link_libraries(${TARGET_NAME} bitxorcore.extensions bitxorcore.plugins bitxorcore.model bitxorcore.validators)
