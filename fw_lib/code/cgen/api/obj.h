/**************************************************************************************************
 * 
 * \file obj.h
 * 
 * \brief Generic API object code generator
 * 
 * \author Marko Gulin
 * 
 *************************************************************************************************/

#ifndef _CGEN_API_OBJ_H
#define _CGEN_API_OBJ_H

/**************************************************************************************************
 * 
 * \brief Generates new private variables structure definition
 * 
 * \param TYPE private variables structure type
 * \param ... zero or more structure members separated by semicolon
 * 
 * \details Private variables structure definition must contain object identifier and number of
 * object variables. For example:
 * 
 *      OBJ_PRIV_STRUCT(PRIV,
 *          void var_1;
 *          void var_2;
 *      )
 * 
 * expands to
 * 
 *      struct PRIV {
 *          const enum obj_id id;
 *          const uint16_t size;
 *          void var_1;
 *          void var_2;
 *      }
 * 
 *************************************************************************************************/
#define OBJ_PRIV_STRUCT(TYPE, ...)                                                              \
    struct TYPE {                                                                               \
        const enum obj_id id;                                                                   \
        const uint16_t size;                                                                    \
        __VA_ARGS__                                                                             \
    }

/**************************************************************************************************
 * 
 * \brief Generates new static private variables structure
 * 
 * \param TYPE private variables structure type
 * \param NAME private variables name
 * \param OBJ corresponding object structure type
 * \param ID object identifier from obj_id enumeration
 * \param ... optional initializers
 * 
 *************************************************************************************************/
#define OBJ_PRIV_NEW(TYPE, NAME, OBJ, ID, ...)                                                  \
    static struct TYPE NAME = {                                                                 \
        .id = ID,                                                                               \
        .size = (sizeof(struct OBJ) - sizeof(void *)) / sizeof(void *),                         \
        __VA_ARGS__                                                                             \
    }

/**************************************************************************************************
 * 
 * \brief Generates new generic API object definition
 * 
 * \param TYPE object structure type
 * \param PRIV private variables structure type
 * \param ... zero or more structure members separated by semicolon
 * 
 * \details Object definition must contain exactly one private variable followed by one or more
 * pointer-based object variables. For example:
 * 
 *      OBJ_STRUCT(TYPE, PRIV,
 *          struct OBJ_VAR *var_1;
 *          struct OBJ_VAR *var_2;
 *      )
 * 
 * expands to
 * 
 *      struct TYPE {
 *          struct PRIV *priv;
 *          struct OBJ_VAR *var_1;
 *          struct OBJ_VAR *var_2;
 *      }
 * 
 *************************************************************************************************/
#define OBJ_STRUCT(TYPE, PRIV, ...)                                                             \
    struct TYPE {                                                                               \
        struct PRIV *priv;                                                                      \
        __VA_ARGS__                                                                             \
    }

/**************************************************************************************************
 * 
 * \brief Generates new static generic API object
 * 
 * \param TYPE object structure type
 * \param NAME object name
 * \param PRIV corresponding private variables handler
 * \param ... optional initializers
 * 
 *************************************************************************************************/
#define OBJ_NEW(TYPE, NAME, PRIV, ...)                                                          \
    static struct TYPE NAME = {                                                                 \
        .priv = PRIV,                                                                           \
        __VA_ARGS__                                                                             \
    }

/**************************************************************************************************
 * 
 * \brief Generates prototype for generic API object constructor
 * 
 * \param TYPE object structure type
 * \param ... input arguments
 * 
 * \details For example
 * 
 *      OBJ_PROTOTPE_NEW(obj, void *in1, void *in2, void *in3)
 * 
 * expands to
 * 
 *      struct obj * obj_new(void *in1, void *in2, void *in3)
 * 
 *************************************************************************************************/
#define OBJ_PROTOTYPE_NEW(TYPE, ...)                                                            \
    struct TYPE * TYPE##_new(__VA_ARGS__)

/**************************************************************************************************
 * 
 * \brief Sets structure member to \b NULL
 * 
 * \param NAME structure member name
 * 
 *************************************************************************************************/
#define OBJ_MEMBER_NULL(NAME)                                                                   \
    .NAME = NULL

/**************************************************************************************************
 * 
 * \brief Sets structure member to variable handler
 * 
 * \param NAME structure member name
 * 
 *************************************************************************************************/
#define OBJ_MEMBER_SET(NAME)                                                                    \
    .NAME = &NAME

/**************************************************************************************************
 * 
 * \brief Adds pointer-based variable to object structure definition
 * 
 * \param TYPE variable structure type
 * \param NAME variable name
 * 
 *************************************************************************************************/
#define OBJ_STRUCT_MEMBER(TYPE, NAME)                                                           \
    struct TYPE *NAME

#endif /* _CGEN_API_OBJ_H */
