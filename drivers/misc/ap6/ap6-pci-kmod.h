/**
 * @file ap6-pci-kmod.h
 *
 *  @date 	18 févr. 2013
 *  @author mdelacour
 */

#ifndef AP6_PCI_KMOD_H_
#define AP6_PCI_KMOD_H_


#include <linux/types.h>

#ifdef __INDEXER__
#include <module_indexer_compat.h>
#endif

/**
 * @brief PCI init
 * @return
 */
s32 ap6PciInit(void);


/**
 * @brief PCI exit
 */
void ap6_pci_exit(void);






#endif /* AP6_PCI_KMOD_H_ */
