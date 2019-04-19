/*
 * This file contains functions to manage attributes.
 */
#ifndef NR_ATTRIBUTES_HDR
#define NR_ATTRIBUTES_HDR

#include <stdint.h>

#include "nr_axiom.h"
#include "util_object.h"

/*
 * Attributes
 *
 * Attributes are key/value pairs created by the agent or the user, and
 * attached to the datatypes generated by the agent.
 *
 * Attributes are stored in the following structure:
 */
typedef struct _nr_attributes_t nr_attributes_t;

/*
 * Destinations
 *
 * Each attribute has its own set of data types to which it should be
 * attached.  These are called 'destinations' and are represented
 * using a bitset:
 */
#define NR_ATTRIBUTE_DESTINATION_NONE 0
#define NR_ATTRIBUTE_DESTINATION_TXN_EVENT 1
#define NR_ATTRIBUTE_DESTINATION_TXN_TRACE 2
#define NR_ATTRIBUTE_DESTINATION_ERROR 4
#define NR_ATTRIBUTE_DESTINATION_BROWSER 8
#define NR_ATTRIBUTE_DESTINATION_ALL                                       \
  (NR_ATTRIBUTE_DESTINATION_TXN_EVENT | NR_ATTRIBUTE_DESTINATION_TXN_TRACE \
   | NR_ATTRIBUTE_DESTINATION_ERROR | NR_ATTRIBUTE_DESTINATION_BROWSER)

/*
 * Attribute keys and value string lengths are limited.  If a string exceeds
 * the limit it is truncated.
 */
#define NR_ATTRIBUTE_KEY_LENGTH_LIMIT 255
#define NR_ATTRIBUTE_VALUE_LENGTH_LIMIT 255

/*
 * This is the maximum number of user attributes that will be stored.
 * When this limit is reached, adding a user attribute will have no effect.
 */
#define NR_ATTRIBUTE_USER_LIMIT 64

/*
 * Configuration
 *
 * Attributes can be configured by the customer.  This configuration allows
 * the customer to:
 *
 *   * Prevent any attributes from being attached to a particular destination.
 *     By preventing attributes from being attached to all destinations,
 *     attributes are effectively disabled.
 *
 *   * Change the destinations of attributes.  Each attribute is created with
 *     a default set of destinations which is overriden by this configuration.
 *
 * All configuration is applied identically to attributes created by the
 * user and agent.  They are only kept separately so that collisions can
 * be handled by New Relic's backend rather than in the agent.
 *
 * This attribute configuration is stored in the following structure:
 */
typedef struct _nr_attribute_config_t nr_attribute_config_t;

/*
 * Purpose : Create an empty attribute configuration.
 */
extern nr_attribute_config_t* nr_attribute_config_create(void);

/*
 * Purpose : Disable attribute destinations.
 *
 * Params  : 1. The configuration to modify.
 *           2. The set of destinations to disable.
 *
 * Note    : Disabling a destination will have priority over destination
 *           modification configuration.  In other words, if a destination
 *           is disabled, no attributes will go there no matter what.
 *
 *           If this function is used multiple times, the disabled destinations
 *           will not be overriden:  They will instead be or-ed.
 */
extern void nr_attribute_config_disable_destinations(
    nr_attribute_config_t* config,
    uint32_t disabled_destinations);

/*
 * Purpose : Modify the destinations of a particular attribute, or all
 *           attributes matching a certain prefix.
 *
 * Params  : 1. The configuration to modify.
 *           2. The string to match against the attribute keys.
 *              If this string ends in a '*', this trailing '*' will match
 *              any number of characters.  A '*' in the middle of the string
 *              will be treated as a literal.
 *           3. A set of destinations where matching attribute/s should be
 *              included.
 *           4. A set of destinations where matching attribute/s should be
 *              excluded.
 *
 * Note    : If this function is called multiple times with the same match
 *           string, the modified destinations are not overriden:  They are
 *           instead or-ed.
 *
 *           If the include/exclude destinations conflict, the exclude setting
 *           will have priority.
 *
 *           Because of the trailing wildcard suffix, multiple of these
 *           destination changes may apply to the same attribute.  If this
 *           is the case, the most specific modifier will have priority.
 */
extern void nr_attribute_config_modify_destinations(
    nr_attribute_config_t* config,
    const char* match,
    uint32_t include_destinations,
    uint32_t exclude_destinations);

/*
 * Purpose : Destroy an attribute configuration, freeing all associated memory.
 */
extern void nr_attribute_config_destroy(nr_attribute_config_t** config_ptr);

/*
 * Purpose : Create a new empty attribute store.
 */
extern nr_attributes_t* nr_attributes_create(
    const nr_attribute_config_t* config);

/*
 * Purpose : Create a new attribute.
 */
extern nr_status_t nr_attributes_user_add(nr_attributes_t* ats,
                                          uint32_t default_destinations,
                                          const char* key,
                                          const nrobj_t* value);
extern nr_status_t nr_attributes_agent_add_long(nr_attributes_t* ats,
                                                uint32_t default_destinations,
                                                const char* key,
                                                int64_t lng);
extern nr_status_t nr_attributes_agent_add_string(nr_attributes_t* ats,
                                                  uint32_t default_destinations,
                                                  const char* key,
                                                  const char* str);

/* Convenience testing functions. */
extern nr_status_t nr_attributes_user_add_string(nr_attributes_t* ats,
                                                 uint32_t default_destinations,
                                                 const char* key,
                                                 const char* value);
extern nr_status_t nr_attributes_user_add_long(nr_attributes_t* ats,
                                               uint32_t default_destinations,
                                               const char* key,
                                               int64_t lng);

/*
 * Purpose : Get all of the attributes for a given destination.  If the
 *           destination parameter includes multiple destinations then
 *           any attribute which matches any of the destinations will be
 *           included.
 */
extern nrobj_t* nr_attributes_user_to_obj(const nr_attributes_t* attributes,
                                          uint32_t destination);
extern nrobj_t* nr_attributes_agent_to_obj(const nr_attributes_t* attributes,
                                           uint32_t destination);

/*
 * Purpose : Destroy an attribute store, freeing all associated memory.
 */
extern void nr_attributes_destroy(nr_attributes_t** attributes_ptr);

#endif /* NR_ATTRIBUTES_HDR */
