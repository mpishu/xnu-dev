//
//  kernel-version
//
//  Created by Pishu Mahtani on 2nd November 2023.
//

#include <stdio.h>
#include <mach/mach.h>

#define MAX_VERSION_LENGTH 1024

void getKernelVersion(char *version, size_t maxVersionLength) {
    host_name_port_t host = mach_host_self();

    if (host == MACH_PORT_NULL) {
        fprintf(stderr, "Failed to get host port\n");
        return;
    }

    kern_return_t kr = host_kernel_version(host, version);

    if (kr != KERN_SUCCESS) {
        fprintf(stderr, "Failed to get kernel version (Error: %d)\n", kr);
        return;
    }
}

int main(void) {
    char version[MAX_VERSION_LENGTH];

    // Get the kernel version
    getKernelVersion(version, sizeof(version));

    if (version[0] != '\0') {
        printf("Kernel Version: %s\n", version);
    } else {
        fprintf(stderr, "Kernel version not available\n");
    }

    return 0;
}
