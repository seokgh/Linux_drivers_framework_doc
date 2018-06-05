/*
eMMC Card Analyzer

auther: shigh1005@gmail.com

Example:
 card_status 0xffffffff

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION	"0.1"

struct card_status_field_value {
	int bit_num;
	char value[10];
};
struct card_status_field_value reference[] = {
	{31, "no error", "error"},
	{30, "no error", "error"},
	{29, "no error", "error"},
	{28, "no error", "error"},

	{27, "no error", "error"},
	{26, "not protected", "protected"},
	{25, "card unlocked", "card locked"},
	{24, "no error", "error"},

	{23, "no error", "error"},
	{22, "no error", "error"},
	{21, "success", "failure"},
	{20, "no error", "error"},

	{19, "no error", "error"},
	{18, "no error", "error"},
	{17, "no error", "error"},
	{16, "no error", "error"},

	{15, "not protected", "protected"},
	{14, "enabled", "disabled"},
	{13, "cleared", "set"},

	{12, "idle", "ready", "ident", "stby", "tran", "data", "rcv", "prg", "dis", "reserved"}, /* 12:9 */

	{8, "not empty", "empty"},
	{7, "reserved", "Permanently 0"}, /* 7:6 */

	{5, "disabled", "enable"},
	{4, "reserved", "Permanently 0"}, 	/* 4:0 */
};


struct card_xxx_field {
	int bit_num;
	char name[100];
	int value;
};

struct card_xxx_field card_status[32] = {
/*31*/		{31, "OUT_OF_RANGE", 0},
/*30*/		{30, "ADDRESS_ERROR", 0},
/*29*/		{29, "BLOCK_LEN_ERROR", 0},
/*28*/		{"ERASE_SEQ_ERROR", 0},

/*27*/		{"ERASE_PARAM", 0},	
/*26*/		{"WP_VIOLATION", 0},
/*25*/		{"CARD_IS_LOCKED", 0},
/*24*/		{"LOCK_UNLOCK_FAILED", 0},

/*23*/		{"COM_CRC_ERROR", 0},
/*22*/		{"ILLEGAL_COMMAND", 0},
/*21*/		{"CARD_ECC_FAILED", 0},
/*20*/		{"CC_ERROR", 0},

/*19*/		{"ERROR", 0},
/*18*/		{"UNDERRUN", 0},
/*17*/		{"OVERRUN", 0},
/*16*/		{"CID/CSD_OVERWRITE", 0},

/*15*/		{"WP_ERASE_SKIP", 0},
/*14*/		{"CARD_ECC_DISABLED", 0},
/*13*/		{"ERASE_RESET", 0},
/*12~9*/	{"CURRENT_STATE", 0},
/*08*/		{"BUFFER_EMPTY", 0},

/*07~06*/	{"reserved", 0},
/*05*/		{"APP_CMD", 0},
/*04*/		{"reserved", 0},

/*03~02*/	{"reserved for application specific commands", 0},
/*01~00*/	{"reserved for manufacturer test mode", 0},
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
	char temp[100];

	if( argc < 2 || !strcmp(argv[1], "help") || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")){
		help(argv[0]);
		return 0;
	}

	printf("-----%s start-----\n", __func__);
	for (i=0; i<32; i++) {
		
		printf("%s,%d\n", card_status[i].name, card_status[i].value);
	}	
	printf("-----%s end-----\n", __func__);

	return 0;
}
