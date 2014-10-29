


#ifndef AP6_DRV_INTERFACE_H_
#define AP6_DRV_INTERFACE_H_


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Driver Version
 */
typedef struct{
	u16 minor;	/** version minor	*/
	u16 major;	/**	version major	*/
	char str[128];				/**	version string	*/
}ap6driverVersion;



/**
 * @brief Driver capability descriptor
 */
typedef struct{
	s32 active;					/** active flag		*/
	char name[128];				/** capability name */
}ap6driverCapability;


/**
 * @brief Maximum number of capabilities per driver
 */
#define AP6DRIVER_MAX_CAPABILITIES	10


/**
 * @brief Apissys Driver Info
 */
typedef struct {
	ap6driverVersion version;										/** driver version 		*/
	char name[128];													/** driver name			*/
	ap6driverCapability capability[AP6DRIVER_MAX_CAPABILITIES];		/** driver capabilities	*/
}ap6driverInfo;


#ifdef __cplusplus
	}
#endif //__cplusplus

#endif /* AP6_DRV_INTERFACE_H_ */
