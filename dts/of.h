of.h (kernel-4.9/include/linux)
  macro
    _LINUX_OF_H
    MAX_PHANDLE_ARGS
    of_phandle_iter
    OF_DYNAMIC
    OF_DETACHED
    OF_POPULATED
    OF_POPULATED_BUS
    OF_BAD_ADDR
    to_of_node
    OF_ROOT_NODE_ADDR_CELLS_DEFAULT
    OF_ROOT_NODE_SIZE_CELLS_DEFAULT
    OF_IS_DYNAMIC
    OF_MARK_DYNAMIC
    for_each_of_allnodes_from
    for_each_of_allnodes
    for_each_property_of_node
    OF_RECONFIG_ATTACH_NODE
    OF_RECONFIG_DETACH_NODE
    OF_RECONFIG_ADD_PROPERTY
    OF_RECONFIG_REMOVE_PROPERTY
    OF_RECONFIG_UPDATE_PROPERTY
    of_match_ptr
    of_property_for_each_phandle_with_args
    of_match_ptr
    of_match_node
    of_compat_cmp
    of_prop_cmp
    of_node_cmp
    of_for_each_phandle
    of_property_for_each_u32
    of_property_for_each_string
    for_each_node_by_name
    for_each_node_by_type
    for_each_compatible_node
    for_each_matching_node
    for_each_matching_node_and_match
    for_each_child_of_node
    for_each_available_child_of_node
    for_each_node_with_property
    _OF_DECLARE
    _OF_DECLARE
    OF_DECLARE_1
    OF_DECLARE_1_RET
    OF_DECLARE_2
  typedef
    phandle
    ihandle
    of_init_fn_2
    of_init_fn_1_ret
    of_init_fn_1
  enum
    of_reconfig_change
  struct
    property
    device_node
    of_phandle_args
    of_phandle_iterator
    of_reconfig_data
    of_changeset_entry
    of_changeset
  function
    of_node_init
    of_node_is_initialized
    of_node_is_attached
    of_node_get
    of_node_put
    is_of_node
    of_have_populated_dt
    of_node_is_root
    of_node_check_flag
    of_node_test_and_set_flag
    of_node_set_flag
    of_node_clear_flag
    of_property_check_flag
    of_property_set_flag
    of_property_clear_flag
    of_read_number
    of_read_ulong
    of_node_full_name
    of_find_node_by_path
    of_property_read_u8_array
    of_property_read_u16_array
    of_property_read_u32_array
    of_property_read_u64_array
    of_core_init
    is_of_node
    to_of_node
    of_node_full_name
    of_find_node_by_name
    of_find_node_by_type
    of_find_matching_node_and_match
    of_find_node_by_path
    of_find_node_opts_by_path
    of_find_node_by_phandle
    of_get_parent
    of_get_next_child
    of_get_next_available_child
    of_find_node_with_property
    of_have_populated_dt
    of_get_compatible_child
    of_get_child_by_name
    of_device_is_compatible
    of_device_is_available
    of_device_is_big_endian
    of_find_property
    of_find_compatible_node
    of_property_count_elems_of_size
    of_property_read_u32_index
    of_property_read_u8_array
    of_property_read_u16_array
    of_property_read_u32_array
    of_property_read_u64_array
    of_property_read_string
    of_property_read_string_helper
    of_get_property
    of_get_cpu_node
    of_property_read_u64
    of_property_match_string
    of_parse_phandle
    of_parse_phandle_with_args
    of_parse_phandle_with_fixed_args
    of_count_phandle_with_args
    of_phandle_iterator_init
    of_phandle_iterator_next
    of_phandle_iterator_args
    of_alias_get_id
    of_alias_get_highest_id
    of_machine_is_compatible
    of_console_check
    of_prop_next_u32
    of_prop_next_string
    of_node_check_flag
    of_node_test_and_set_flag
    of_node_set_flag
    of_node_clear_flag
    of_property_check_flag
    of_property_set_flag
    of_property_clear_flag
    of_node_to_nid
    of_numa_init
    of_find_matching_node
    of_property_count_u8_elems
    of_property_count_u16_elems
    of_property_count_u32_elems
    of_property_count_u64_elems
    of_property_read_string_array
    of_property_count_strings
    of_property_read_string_index
    of_property_read_bool
    of_property_read_u8
    of_property_read_u16
    of_property_read_u32
    of_property_read_s32
    of_get_child_count
    of_get_available_child_count
    of_changeset_attach_node
    of_changeset_detach_node
    of_changeset_add_property
    of_changeset_remove_property
    of_changeset_update_property
    of_reconfig_notifier_register
    of_reconfig_notifier_unregister
    of_reconfig_notify
    of_reconfig_get_state_change
    of_device_is_system_power_controller
    of_overlay_create
    of_overlay_destroy
    of_overlay_destroy_all
