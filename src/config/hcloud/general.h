/* Allow retrieval of metadata (such as an iPXE boot script) from
 * Google Compute Engine metadata server.
 */
// #define HTTP_HACK_GCE

/* Based on https://ipxe.org/buildcfg I've enabled everything that
 * seems remotely usefull for debugging on Hetzner Cloud
 */

#define DOWNLOAD_PROTO_HTTPS
#define NET_PROTO_IPV6
#define CERT_CMD
#define CONSOLE_CMD
#define IMAGE_TRUST_CMD
#define IPSTAT_CMD
#define LOTEST_CMD
#define NEIGHBOUR_CMD
#define NSLOOKUP_CMD
#define NTP_CMD
#define PARAM_CMD
#define PCI_CMD
#define PING_CMD
#define POWEROFF_CMD
#define PROFSTAT_CMD
#define REBOOT_CMD
#define TIME_CMD
#define VLAN_CMD

