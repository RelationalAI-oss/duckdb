add_subdirectory(expression_executor)
add_subdirectory(merge_join)
add_subdirectory(nested_loop_join)
add_subdirectory(operator)
add_subdirectory(physical_plan)
add_subdirectory(index/art)

add_library(duckdb_execution OBJECT
            aggregate_hashtable.cpp
            column_binding_resolver.cpp
            expression_executor.cpp
            join_hashtable.cpp
            physical_operator.cpp
            physical_plan_generator.cpp
            window_segment_tree.cpp)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:duckdb_execution>
    PARENT_SCOPE)
