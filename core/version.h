#ifndef __VERSION_H__
#define __VERSION_H__
#if defined(__cplusplus)
extern "C" {
#endif

/**
 * version format:
 *      0x00AAIIPP
 *      AA: major version
 *      II: minor version
 *      PP: patch
 */
#define IMGANA_VERSION  0x00000000

unsigned int imgana_get_version(void);
const char *imgana_get_version_str(void);

#if defined(__cplusplus)
}
#endif
#endif /* __VERSION_H__ */

