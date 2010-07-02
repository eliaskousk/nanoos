/*
 * kernel_drivers/adi_buses/pci/pcibus_names_gen.c
 *
 * Copyright (c) 2003-2005 Lukasz Dembinski <dembol@nasa.com.pl>
 * All Rights Reserved
 * 
 * Date:	2005/01
 * Author: 	Lukasz Dembinski
 * Info:	pcibus_names_gen.c core file
 * Contact:	mailto: <dembol@nasa.com.pl>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <memory.h>

#define MAX_BUF_LEN	0x200
#define MAX_VENDORS	2000
#define MAX_DEVICES	10000
void usage(char *s)
{
	printf("Usage %s <pci.ids file>\n",s);
	printf("This util is borrowed from agnix OS\n");
	printf("See the file  %s.cfor details about the author and copy right\n");
	exit(0);
}
int buf_check(char *buf, int len)
{
    int i, j;

    for (i = 0; i < len; i++) {
	if (buf[i] == '"') {
	
	    for (j = len; j >= i + 1; j--) {
		buf[j] = buf[j - 1];
	    }
	
	    buf[i] = '\\';
	    buf[i + 1] = '"';
	    i++;
	    len++;
	}
    }
    
    return len;
}

char vendor_names[MAX_VENDORS][MAX_BUF_LEN];
int  vendor_names_count = 0;

char device_names[MAX_DEVICES][MAX_BUF_LEN];
int  device_names_count = 0;


int vendor_check_redund(char *name) {
    int i;
    
    for (i = 0; i < vendor_names_count; i++) {
	if (!strcmp(vendor_names[i], name))
	    return 1;
    }
    
    return 0;
}

int device_check_redund(char *name) {
    int i;
    
    for (i = 0; i < device_names_count; i++) {
	if (!strcmp(device_names[i], name))
	    return 1;
    }
    
    return 0;
}


int main(int argc, char *argv[])
{
    FILE *fin;
    FILE *fout_v, *fout_d, *fout_h;
    char buf[MAX_BUF_LEN];
    char *buf_2;
    unsigned long vendor_id;
    unsigned long device_id;
    char vendor_name[MAX_BUF_LEN];
    char device_name[MAX_BUF_LEN];
    char define_name[MAX_BUF_LEN];
    char tmp_name[MAX_BUF_LEN];
    int tmp_name_id;
    int i;
    char name[MAX_BUF_LEN];
/*added by ashok for NanoOS*/
	if(argc<2)
		usage(argv[0]);
	fin = fopen(argv[1],"r");
/*addition ends*/
    /*fin  = fopen("pcibus_names.ids", "r"); original */

    fout_v = fopen("pcibus_vendor_names.c", "w");    
    fout_d = fopen("pcibus_device_names.c", "w");    
    fout_h = fopen("pcibus_define_names.h", "w");    

    fprintf(fout_v, "/* \n");
    fprintf(fout_v, " * kernel_drivers/adi_buses/pci/pcibus_vendor_names.c\n");
    fprintf(fout_v, " * \n");
    fprintf(fout_v, " * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@nasa.com.pl>\n");
    fprintf(fout_v, " * All Rights Reserved\n");
    fprintf(fout_v, " * \n");
    fprintf(fout_v, " * Date:	2004/06\n");
    fprintf(fout_v, " * Author: 	Lukasz Dembinski\n");
    fprintf(fout_v, " * Info:	pcibus_vendor_names.c core file\n");
    fprintf(fout_v, " * Contact:	mailto: <dembol@nasa.com.pl>\n");
    fprintf(fout_v, " * \n");
    fprintf(fout_v, " */\n\n");

    fprintf(fout_v, "#include <agnix/agnix.h>\n");
    fprintf(fout_v, "#include <agnix/adi/adi.h>\n\n");
    fprintf(fout_v, "struct pcibus_vendor_name_s pcibus_vendor_names[] = {\n");

    while(fgets(buf, MAX_BUF_LEN - 1, fin)) {
    	if (buf[0] == '#')
	    continue;

	buf[strlen(buf) - 1] = 0;

	if (buf[0] != '\t') {
	    if (sscanf(buf, "%04x", &vendor_id) > 0) {
		fprintf(fout_v, "\t{ 0x%04x, \"%s\" }, \n", vendor_id, buf + 6);
    	    	    
		if (vendor_id == 0xffff)
		    break;
	    }
	}
	
	memset(buf, 0, MAX_BUF_LEN);	
    }
    fprintf(fout_v, "};\n\n");

    fseek(fin, 0, 0);
    

    fprintf(fout_d, "/* \n");
    fprintf(fout_d, " * kernel_drivers/adi_buses/pci/pcibus_device_names.c\n");
    fprintf(fout_d, " * \n");
    fprintf(fout_d, " * Copyright (c) 2003-2004 Lukasz Dembinski <dembol@nasa.com.pl>\n");
    fprintf(fout_d, " * All Rights Reserved\n");
    fprintf(fout_d, " * \n");
    fprintf(fout_d, " * Date:	2004/06\n");
    fprintf(fout_d, " * Author: 	Lukasz Dembinski\n");
    fprintf(fout_d, " * Info:	pcibus_device_names.c core file\n");
    fprintf(fout_d, " * Contact:	mailto: <dembol@nasa.com.pl>\n");
    fprintf(fout_d, " * \n");
    fprintf(fout_d, " */\n\n");

    fprintf(fout_d, "#include <agnix/agnix.h>\n");
    fprintf(fout_d, "#include <agnix/adi/adi.h>\n\n");
        
    fprintf(fout_d, "struct pcibus_device_name_s pcibus_device_names[] = {\n");

    while(fgets(buf, MAX_BUF_LEN - 1, fin)) {
	if (buf[0] == '#')
	    continue;

	buf[strlen(buf) - 1] = 0;

	if (buf[0] != '\t') {
	    if (sscanf(buf, "%04x", &vendor_id) > 0) {
	        if (vendor_id == 0xffff)
		    break;
	    }
	} else 
	if (buf[1] != '\t') {
	    if (sscanf(buf, "%04x", &device_id) > 0) {
		buf_check(buf + 7, strlen(buf) - 7);
	    
		fprintf(fout_d, "\t{ 0x%04x, 0x%04x, \"%s\" }, \n", vendor_id, device_id, buf + 7);
    	    }
	}
	
	memset(buf, 0, MAX_BUF_LEN);	
	
    }
    fprintf(fout_d, "};\n\n");


    fprintf(fout_h, "#ifndef _AGNIX_ADI_DEFINE_NAMES_H_\n");
    fprintf(fout_h, "#define _AGNIX_ADI_DEFINE_NAMES_H_\n\n");

    fseek(fin, 0, 0);

    while(fgets(buf, MAX_BUF_LEN - 1, fin)) {
    	if (buf[0] == '#')
	    continue;

	memset(name, 0, MAX_BUF_LEN);	
	buf[strlen(buf) - 1] = 0;

	if (buf[0] != '\t') {
	    
	    if (sscanf(buf, "%04x", &vendor_id) > 0) {
		buf_2 = buf + 6;
		for (i = 0; i < strlen(buf_2); i++) {
		
		    if (buf_2[i] == ' ' || buf_2[i] == ',')
			break;
		    
		    if (buf_2[i] == '\'' || buf_2[i] == ':' || buf_2[i] == '.' || buf_2[i] == '+' || buf_2[i] == '-' || buf_2[i] == '/' || buf_2[i] == '\\')
			buf_2[i] = '_';
		    
		    if (buf_2[i] == '&')
		    	buf_2[i] = 'N';
		
		    if (buf_2[i] == '?')
		    	buf_2[i] = 'x';

		    if (buf_2[i] == '[' || buf_2[i] == ']')
		    	buf_2[i] = '_';

		    if (buf_2[i] >= 'a' && buf_2[i] <= 'z')		
			buf_2[i] = buf_2[i] - 32;
		}
		
		strncpy(name, buf_2, i);
		
		if (vendor_check_redund(name))
		    continue;

		strcpy(vendor_names[vendor_names_count++], name);
				
		sprintf(define_name, "#define PCIBUS_VENDOR_%s_ID", name);
		
		fprintf(fout_h, "%-040s\t0x%04x\n", define_name, vendor_id);
    	    	    
		if (vendor_id == 0xffff)
		    break;
	    }
	}
	
	memset(buf, 0, MAX_BUF_LEN);	
    }


    fprintf(fout_h, "\n\n");

    fseek(fin, 0, 0);

    while(fgets(buf, MAX_BUF_LEN - 1, fin)) {
    	if (buf[0] == '#')
	    continue;

	memset(name, 0, MAX_BUF_LEN);	
	buf[strlen(buf) - 1] = 0;


	if (buf[0] != '\t') {
	    if (sscanf(buf, "%04x", &vendor_id) > 0) {
	        if (vendor_id == 0xffff)
		    break;
	    }
	} else 
	if (buf[1] != '\t') {
	    if (sscanf(buf, "%04x", &device_id) > 0) {
		buf_check(buf + 7, strlen(buf) - 7);
	    
	    if (sscanf(buf, "%04x", &vendor_id) > 0) {
		buf_2 = buf + 7;
		for (i = 0; i < strlen(buf_2); i++) {
		
		    if (buf_2[i] == ' ' || buf_2[i] == ',' || buf_2[i] == '\\' || buf_2[i] == '/' || buf_2[i] == '!')
			break;

		    if (buf_2[i] == '\'' || buf_2[i] == ':' || buf_2[i] == '.' || buf_2[i] == '+' || buf_2[i] == '-' || buf_2[i] == '/' || buf_2[i] == '\\')
			buf_2[i] = '_';

		    if (buf_2[i] == '&')
		    	buf_2[i] = 'N';

		    if (buf_2[i] == '?')
		    	buf_2[i] = 'x';

		    if (buf_2[i] == '[' || buf_2[i] == ']')
		    	buf_2[i] = '_';

		
		    if (buf_2[i] >= 'a' && buf_2[i] <= 'z')		
			buf_2[i] = buf_2[i] - 32;

		    if (buf_2[i] == '(')
		    	buf_2[i] = '_';

		    if (buf_2[i] == ')')
		    	buf_2[i] = '_';

		}
		
		strncpy(name, buf_2, i);
		
		strcpy(tmp_name, name);
		tmp_name_id = 1;
		
		while(device_check_redund(tmp_name)) {
		    sprintf(tmp_name, "%s_%d", name, tmp_name_id++);
		}

		strcpy(device_names[device_names_count++], tmp_name);
				
		sprintf(define_name, "#define PCIBUS_DEVICE_%s_ID", tmp_name);
		
		fprintf(fout_h, "%-040s\t0x%04x\n", define_name, device_id);

		}
	    }
	}
	
	memset(buf, 0, MAX_BUF_LEN);	
    }

    
    fprintf(fout_h, "\n#endif\n\n");
    
    fclose(fin);
    fclose(fout_v);
    fclose(fout_d);
    fclose(fout_h);
}
