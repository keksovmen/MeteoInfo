#ifndef ETL_PROFILE_H
#define ETL_PROFILE_H

#define ETL_NO_EXCEPTIONS
// #define ETL_VERBOSE_ERRORS
#define ETL_NO_RTTI
#define ETL_NO_STL
#define ETL_CHECK_PUSH_POP
// #define ETL_DEBUG


#define ETL_TARGET_DEVICE_GENERIC
#define ETL_TARGET_OS_NONE
#define ETL_COMPILER_GENERIC
#define ETL_CPP11_SUPPORTED 1
#define ETL_CPP14_SUPPORTED 1
#define ETL_CPP17_SUPPORTED 1
#define ETL_NO_NULLPTR_SUPPORT 0
#define ETL_NO_LARGE_CHAR_SUPPORT 0
#define ETL_CPP11_TYPE_TRAITS_IS_TRIVIAL_SUPPORTED 1

// Essential size optimizations
// #define ETL_NO_STL
// #define ETL_NO_CPP_STRINGS
// #define ETL_NO_LARGE_CHAR_SUPPORT 0
// #define ETL_NO_WIDE_CHAR_SUPPORT 0
// #define ETL_NO_NULLPTR_SUPPORT 0
// #define ETL_NO_EXCEPTIONS
// #define ETL_NO_RTTI

// // Container size optimizations
#define ETL_VECTOR_REPAIR_ENABLE 0
#define ETL_DEQUE_REPAIR_ENABLE 0
#define ETL_LIST_REPAIR_ENABLE 0
#define ETL_QUEUE_REPAIR_ENABLE 0
#define ETL_STACK_REPAIR_ENABLE 0

// // Algorithm optimizations
// #define ETL_ALGORITHM_SORT_REPAIR_ENABLE 0
// #define ETL_ALGORITHM_FIND_REPAIR_ENABLE 0

// // String optimizations
// #define ETL_ISTRING_REPAIR_ENABLE 0
// #define ETL_ISTREAM_REPAIR_ENABLE 0
#define ETL_IN_UNIT_TESTS 0
// #define ETL_NOT_USING_STCPP  // If not using embedded C++

// // Disable verbose errors
#define ETL_VERBOSE_ERRORS 0
#define ETL_LOG_ERRORS 0
#define ETL_LOG_DEBUG 0

// // FSM optimizations (if using finite state machine)
#define ETL_FSM_VERBOSE_DEBUGGING 0

#endif
