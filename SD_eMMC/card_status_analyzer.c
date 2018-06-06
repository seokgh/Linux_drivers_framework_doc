/*
	eMMC Card Status Analyzer

auther: 

Example:
	card_status 0xffffffff

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION	"0.1"


struct card_status {
	int bit_num;
	char field_name[100];
	char value[80][60];
	int index;
};

struct card_status status_description[] = {
	{31, "ADDRESS_OUT_OF_RANGE",	{"no error", "error",}},
	{30, "ADDRESS_MISALIGN    ",	{"no error", "error",}},
	{29, "BLOCK_LEN_ERROR     ",	{"no error", "error",}},
	{28, "ERASE_SEQ_ERROR     ",	{"no error", "error",}},
	
	{27, "ERASE_PARAM         ",	{"no error", "error",}},
	{26, "WP_VIOLATION        ",	{"no error", "error",}},
	{25, "CARD_IS_LOCKED      ",	{"unlocked", "locked",}},
	{24, "LOCK_UNLOCK_FAILED  ",	{"no error", "error",}},
	
	{23, "COM_CRC_ERROR       ",	{"no error", "error",}},
	{22, "ILLEGAL_COMMAND     ",	{"no error", "error",}},
	{21, "CARD_ECC_FAILED     ",	{"success", "failure",}},
	{20, "CC_ERROR            ",	{"no error", "error",}},
	
	{19, "ERROR               ",	{"no error", "error",}},
	{18, "Obsolete            ",	{"",}},
	{17, "Obsolete            ",	{"",}},
	{16, "CID/CSD_OVERWRITE   ",	{"no error", "error",}},
	
	{15, "WP_ERASE_SKIP       ",	{"not protected", "protected",}},
	{14, "CARD_ECC_DISABLED   ",	{"enabled", "disabled",}},
	{13, "ERASE_RESET         ",	{"cleared", "set",}},
	{12, "CURRENT_STATE       ",	{"idle", "ready", "ident", "stby", "tran", "data", "rcv", "prg", "dis", "reserved",}}, /* 12:9 */
	
	{8,  "READY_FOR_DATA      ",	{"not ready", "ready",}},
	{7,  "SWITCH_ERROR        ",	{"no error", "error",}},
	{6,  "EXCEPTION_EVENT     ",	{"no event", "an exception event has occurred",}},
	{5,  "APP_CMD             ",	{"disabled", "enable"}},
	
	{4,  "Reserved            ",	{"",}},
	{3,  "Reserved            ",	{"",}},	/*03~02*/
	{1,  "Reserved            ",	{"",}},	/*01~00*/
};

static void help(char *np)
{
	struct Command *cp;

	printf("Usage:\n");
	printf("\n\t%s help|--help|-h\n\t\tShow the help.\n",np);
	printf("\n\t%s <cmd> --help\n\t\tShow detailed help for a command or subset of commands.\n",np);
	printf("\n%s\n", VERSION);
}

int main(int argc, char **argv)
{
	int i;
	int value = strtoul(argv[2], NULL, 0);
	int index[50] = {0};

	if( argc < 2 || !strcmp(argv[1], "help") || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")){
		help(argv[0]);
		return 0;
	}

	printf("-----%s start-----\n", __func__);
	for (i=0; i<sizeof(status_description)/sizeof(status_description[0]); i++) {
		
		//current_status
		if ( status_description[i].bit_num == 12) {
			status_description[i].index = (value >> 9) & 0x0000000F;

			if (status_description[i].index > 9)
				status_description[i].index = 9;
				
		} else {
			status_description[i].index = (value >> status_description[i].bit_num) & 0x00000001;
		}

		//obsolete,reserved




		printf("[%s]	[%s]\n",  status_description[i].field_name, status_description[i].value[status_description[i].index]);
	}
	
	printf("-----%s end-----\n", __func__);

	return 0;
}
