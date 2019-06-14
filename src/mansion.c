/*
 * Mystery Mansion
 *
 * Explore the mansion, solve the mystery!
 *
 * C version of the HP1000 (and later HP3000) text adventure game.  The 
 * original was Fortran IV and somewhat obfuscated.   Revision 16 of the
 * original source was the last HP1000 version; Revision 17 was a port of
 * that to the HP3000.  This C port is a port of the latter, with comments
 * from the former.  For proper formatting, view this file with your
 * tabstop set to 8.
 *
 * This software is provided ``as is'' and any express or implied warranties, 
 * including, but not limited to, the implied warranties of merchantability 
 * and fitness for a particular purpose are disclaimed.  
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <fcntl.h>

/*
 * Only unix boxes want the standard unix header gunk
 */
#ifdef UNIX
#include <unistd.h>
#endif

#include "mansion.h"

char IVRBX[9][8];
char IVRB[][16] = {
	"        ", "GO      ", "CLIMB   ", "ENTER   ", "XIMOW   ", "XIWOM   ",
	"MOWIX   ", "WOMIX   ", "WIMOX   ", "WOXIM   ", "XIMOW   ", "MOXIW   ",
	"DIG     ", "FOLLOW  ", "        ", "        ", "        ", "        ",
	"QUIT    ", "STOP    ", "SCORE   ", "GET     ", "TAKE    ", "DROP    ",
	"GIVE    ", "LOOK    ", "READ    ", "FIND    ", "LIST    ", "LIGHT   ",
	"TURN    ", "HELP    ", "OPEN    ", "CLOSE   ", "SNUFF   ", "BLOW    ",
	"EAT     ", "RUB     ", "BEAM    ", "SWEEP   ", "CALL    ", "MOVE    ",
	"ANSWER  ", "FEED    ", "DISPLAY ", "ATTACK  ", "KILL    ", "SUSPEND ",
	"RESTORE ", "DRINK   ", "MYSTERY ", "BRIBE   ", "QUESTION", "LISTEN  ",
	"UNLOCK  ", "LOCK    ", "SHOOT   ", "OIL     ", "RECORD  ", "BREAK   ",
	"PLACE   ", "SLEEP   ", "        ", "        ", "        ", "        ",
	"        ", "        ", "        ", "        ", "        ", "        ",	
	"WALK    ", "PUT     ", "PICK    ", "        ", "        ", "        ",
	"        ", "        ", "        ", "        ", "        ", "        ",
	"        ", "        ", "        ", "        ", "        ", "        "
};

char *IRNM[] = {
	"", 
	"BUTLER'S ROOM    ", "FURNACE ROOM    ", 
	"DARK PIT         ", "SECRET PASSAGE  ", "CREEPY CRYPT    ",
	"WINE CELLAR      ", "STORAGE ROOM    ", "DAMP DUNGEON    ",
	"FOOD CELLAR      ", "LIVING ROOM     ", "ENTRANCE HALL   ",
	"GAME ROOM        ", "BALL ROOM       ", "DINING HALL     ",
	"CHARMING CHAPEL  ", "LIBRARY         ", "KITCHEN         ",
	"CLOSET           ", "MASTER BEDROOM  ", "BELL TOWER      ",
	"HAUNTED HALLWAY  ", "LARGE BEDROOM   ", "MAID'S ROOM     ",
	"TWISTY MAZE      ", "COLD CORRIDOR   ", "SHORT CUT       ",
	"TRICKY TREASURY  ", "MOLE MAZE       ", "MOLE'S VAULT    ",
	"MOLE CAVE        ", "VEXING VERANDA  ", "GARDEN GATE     ",
	"GARDEN EXIT      ", "GARGOYLE GARDEN ", "GRASSY MEADOW   ",
	"FOUL FOUNTAIN    ", "WITCHING WELL   ", "BROKEN BRIDGE   ",
	"POLLUTED POND    ", "COZY COTTAGE    ", "FIRE ESCAPE     ",
	"STRANGE STREAM   ", "AWKWARD ATTIC   ", "LABORATORY      ",
	"HIDEOUS HIGHWAY  ", "FRONT PORCH     ", "DREARY DRIVEWAY ",
	"MAIN GATE        ", "ROADWAY         ", "BACK GATE       ",
	"DENSE WOODS      ", "WOODS           ", "TREASURE TREK   ",
	"DEN OF DEATH     ", "TROLL TRAP      ", "BATTY BATHROOM  "
};

/*
 * IROM (rooms)
 *   == LIGHT/POINTS/ROOM ID (X/YY/ZZ)
 *         LIGHT == 0 for dark, 1 for closed curtain, 2 for open curtain or lit
 *        POINTS == points player obtains for finding this room
 *       ROOM ID == unique ID number of this room
 */
int IROM[] = {
	0,			/* filler                                     */
	5201,5202,5203,		/* butler's room, furnace room, dark pit      */
	5804,5205,5206,		/* secret passage, creepy crypt, wine cellar  */
	5207,5208,5209,		/* storage room, damp dungeon, food cellar    */
	15210,25211,15212,	/* living room, entrance hall, game room      */
	5804,5213,15214,	/* secret passage, ball room, dining hall     */
	5215,15816,15217,	/* charming chapel, library, kitchen          */
	5218,15219,25220,	/* closet, master bedroom, bell tower         */
	5804,5221,15222,	/* secret passage, haunted hallway, l. b.room */
	15223,15221,5218,	/* maid's room, haunted hallway, closet       */
	5824,5024,5024,		/* twisty maze, twisty maze, twisty maze      */
	5124,5024,5124,		/* twisty maze, twisty maze, twisty maze      */
	5024,25245,5225,	/* t. maze, hideous highway, cold corridor    */
	5125,3025,5225,		/* c. corridor, c. corridor, c. corridor      */
	5826,8027,5828,		/* short cut, tricky treasury, mole maze      */
	5028,5028,5028,		/* mole maze, mole maze, mole maze            */
	5128,5028,5028,		/* mole maze, mole maze, mole maze            */
	5028,5028,5028,		/* mole maze, mole maze, mole maze            */
	8029,5830,5231,		/* mole's vault, mole cave, vexing veranda    */
	5232,5233,5233,		/* garden gate, garden exit, garden exit      */
	5034,5034,5034,		/* gargoyle garden, g. garden, g. garden      */
	5034,5034,5034,		/* gargoyle garden, g. garden, g. garden      */
	5234,5234,5234,		/* gargoyle garden, g. garden, g. garden      */
	5234,5234,5234,		/* gargoyle garden, g. garden, g. garden      */
	5234,6535,6535,		/* gargoyle garden, grassy meadow, g. meadow  */
	5236,5236,5237,		/* foul fountain, f. fountain, witching well  */
	5237,5237,37,		/* witching well, w. well, w. well            */
	8037,37,5238,		/* witching well, w. well, broken bridge      */
	5238,39,6540,		/* broken bridge, polluted pond, cozy cottage */
	5241,8041,6543,		/* fire escape, fire escape, awkward attic    */
	6518,8044,5156,		/* closet, laboratory, batty bathroom         */
	5246,6547,5248,		/* front port, dreary driveway, main gate     */
	5249,5250,5249,		/* roadway, back gate, roadway                */
	5152,5151,5842		/* woods, dense woods, strange stream         */
};

char *IRSN[] = {
	"        ", "CORPSE  ", "LADY    ", "BUTLER  ", "MOLE    ", "VAMPIRE ",
	"WOODSMAN", "GARDENER", "MAID    ", "COOK    ", "WEREWOLF", "ELF     ",
	"HUNTER  ", "DWARF   ", "MASTER  ", "WOLF    ", "WARRIOR ", "        "
};

/*
 * IRES (NPC states -- "residents")
 *   == HEALTH/STRENGTH/ROOM (XX/Y/ZZ)
 *         HEALTH == wounded state, from 0 (healthy) to 10 (dead)
 *       STRENGTH == relative strength of the character
 *           ROOM == where he/she/it is
 */
int IRES[] = { 
	0, 	/* filler                                  */
	19,	/* corpse: defaults to closet              */
	424,	/* lady: strength 4, large bedroom         */
	911,	/* butler: strength 9, entrance hall       */
	43,	/* mole: strength 0, mole maze             */
	300,	/* vampire: strength 3, no initial default */
	894,	/* woodsman: strength 8, west roadway      */
	763,	/* gardener: strength 7, gargoyle garden   */
	525,	/* maid: strength 5, maid's room           */
	718,	/* cook: strength 7, kitchen               */
	0,	/* werewolf: not around at start of game   */
	1870,	/* elf: strength 8, gargoyle garden        */
	996,	/* hunter: strength 9, east roadway        */
	330,	/* dwarf: strength 3, twisty maze          */
	920,	/* master: strength 9, master bedroom      */
	71,	/* wolf: grassy meadow                     */
	0,	/* warrior:                                */
	0	/* nobody                                  */
};

char *IPRP[] = {
	"        ", "IN      ", "ON      ", "BY      ", "AT      ", "TO      ",
	"OVER    ", "FROM    ", "OUT     ", "THE     ", "OFF     ", "BOOTY   ",
	"A       ", "WITH    ", "        ", "        ", "        ", "        "
};

char *IDTN[] = { 
	"        ", "NORTH   ", "EAST    ", "SOUTH   ", "WEST    ", "UP      ", 
	"DOWN    ", "BACKWARD", "LEFT    ", "FORWARD ", "RIGHT   ", "BACK    "
};

/*
 * IVEN (inventory items)
 *   == HOLDER/SET/WEIGHT/ROOM (W/X/Y/ZZ)
 *       HOLDER == ID of NPC or player (0 == nobody, 1 == NPC, 2 == Player)
 *          SET == object dependent setting (e.g. for candle, 1 == lit)
 *       WEIGHT == how much the object weighs (duh)
 *         ROOM == ID of room it's in or NPC carrying it
 */
int IVEN[] = {
	0, /* filler */
	0,121,220,493,1316,126,441,233,217,5218,9396,7312,
	125,5308,295,520,152,105,1216,114,217,193,789,309,319,789,0,
	1184,1214,312,118,394,202,17213,625,0,0,0,0,
	0,0,0,0,0,14416,1101,0,0,10216,14316,0,0,20100,0,0,0,0,0,0,
	16,0,0,0,24,41,41,14,27,7,5,0,
	10,11,0,12,10,0,0,0,0,0,0,52,43,93,0,0,17,14,
	91,0,0,0,15,15,15,15,15,0,51,51,51,53,0,0,0,0
};

/*
 * IXT (map)
 *   == LOCK/DEST-ROOM-ID/FROM-ROOM-ID (X/YY/ZZ)
 *          LOCK == 0 for no lock, 1 for locked door, 2 for unlocked
 *  DEST-ROOM-ID == the room ID of the room that can be reached from here
 *  FROM-ROOM-ID == the room ID of the room we're in ("here")
 * 
 * Broken up in six blocks.  If the ID number of the FROM-ROOM-ID matches the
 * ID number of the room the player is in, then by going the direction of 
 * the block, he/she will enter the room with the DEST-ROOM-ID.   Within each
 * block, the rooms are unordered -- that is, the array index does not matter,
 * only the FROM-ROOM-ID.  Thus, every move requires a scan of all 60 
 * array elements in each block to determine if the move is possible.
 */
int IXT[] = {
            0,   /* filler */
	/* NORTH */
	  401,   502,   704, 20906,  1310,  1411,  9562,  1613, 11714,  1815,
	 2219, 22320,  2421,  2522,  2623, 22724,   328,  3429,  2085, 24036,
	 2831,  4342,  4645,  4647,  4849, 15417,  5554,  5856,  5100,  4600,
	    0,     0,  6157,  6458,  7359,  7460,  6561,  7164,  7265,  6866,
	 6967,  6268,  6369,  6871,  6972,    95,  9335,  8176,  8277,  6281,
	 6382,  1191,  9792, 29293,  9894,  9596,     0,  9899,  9897,  9798,
	/* EAST */
	20201,   302,   504, 20807,  1110,  1211,  1413,  1514, 22019,  1716,
	 2322, 22423, 22625,    35,  2829,  2930,  2931,   436,  3637,  3738,
	 3839,  5342,  4443,  4644,  4445,  4446,  4548,  4350,  4200,  4700,
	29026,     0,  8890,     0,  6055,  5456,  5958,  5559,  6160,  7062,
	 6963,  7364,  6765,  7166,  8368,  6769,  6370,  7671,  6772,  7573,
	 6574,  7475,  7277,  9192,  9693,    21,  9695,    96,  9497,  9898,
	/* SOUTH */
	  104,   205,   407, 20609,  1013,  1114,  6295,  1316, 11417,  1518,
	 1922, 22023,  2124,  2225,  2326, 22427,  8520,  3228,  3229,  3640,
	 3033,  4344,  4847,  5150,  4251, 11754,  5455,  5658,  4300,  4800,
	    0,     0,  5761,  8162,  8263,  5864,  6165,  6466,  6567,  7168,
	 7269,  8370,  6471,  6572,  5973,  6074,  3593,  3535,  7681,  7782,
	 2488,  8889,  9111,     0,     0,  9394,  9799,  9396,  9897,  9898,
	/* WEST */
	20102,   203,   405, 20708,  1011,  1112,  1314,  1415, 21920,  1617,
	 2223, 22324, 22526,  2928,  3330,  2932,  9766,  3604,  3736,  3837,
	 3938,  5043,  4845,  5247,  4948,  5149,  4950,  4951,  4400,  4900,
	22690,  9088,     0,  5955,  5457,  5859,  5560,  6061,  6862,  7063,
	 6664,  7465,  7267,  6668,  8369,  6270,  6671,  7772,  6473,  7574,
	 7375,  7176,  9291, 29392,  9493,  9794,  9895,  9835,  9997,  9898,
	/* UP */
	 1203,  1304,  1607,  1809,  2112,  2213,  2314,  3128,  3129,  2832,
	    0,  4039,  3941,  4544,  4746,  4748,  5051,  4752,  4500,  5000,
	    0,     0,     0,   753,  8687,     0,  8499,  9884,  2934,     0,
	    0,     0,  9933,  3839,  3738,  3637,  3130,     0,     0,     0,
	 8990,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	/* DOWN */
	  312,   413,   716,   918,  1221,  1322,  1423,  2803,  3028,  3029,
	 2830,  3031,  4139,  3940,  5142,  4243,  4546,  5049,  4253,  5307,
	 9984,     0,     0,     0,     0,     0,     0,     0,  5100,  5100,
	    0,  1987,  3399,  3938,  3837,  3736,     0,     0,     0,     0,
	    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	 9089,     0,     0,     0,     0,     0,     0,     0,     0,     0
};

char *ICLS[] = {
	"        ", "PITIFUL ", "POOR    ", "UNLUCKY ", "NOVICE  ",
	"FAIR    ", "AMATEUR ", "GOOD    ", "LUCKY   ",
	"SKILLFUL", "SUPERB  ", "MASTER  ", "SUPER   "
};

/*
 * ITST (game state)
 */
int ITST[] = {
	0, 	/*  0: filler */
	93,	/*  1: player's location */
	45,	/*  2: player's score */
	999,	/*  3: player's possible total score */
	1000,	/*  4: which way player is facing (n/s/e/w) */
	0,	/*  5: current gametime (game hours * 25) */
	0,	/*  6: primary-illumination level-remaining */
	0,	/*  7: secondary-illumination level-remaining */
	0,	/*  8: scene of the murder index */
	3,	/*  9: murderer index */
	0,	/* 10: murder weapon index */
	6,
	0,
	0,
	93,
	17,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	1,
	0,
	0,
	0,
	29,
	12,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	1,
	0,
	0
};

char *ITEM[] = {
	"        ",
	"RING    ", "KEYS    ", "CLOCK   ", "LANTERN ", "CROSS   ", 
	"COMPASS ", "TREASURE", "BATTERY ", "GUN     ", "KNIFE   ", 
	"AXE     ", "SWORD   ", "VIAL    ", "CLUB    ", "ROPE    ", 
	"CHAIR   ", "AMULET  ", "TALISMAN", "CANDLE  ", "BULLET  ", 
	"BOOK    ", "NOTE    ", "XMITTER ", "FOOD    ", "BROOM   ", 
	"RECEIVER", "TORCH   ", "MAP     ", "WEDGE   ", "GLOBE   ", 
	"MATCH   ", "SHOVEL  ", "OILCAN  ", "HATCHET ", "CHEST   ", 
	"PEARLS  ", "EMERALD ", "COINS   ", "GOBLET  ", "DIAMONDS", 
	"JEWELRY ", "NECKLACE", "CROWN   ", "DARTS   ", "SHIELD  ", 
	"KEY     ", "BATTERY ", "GAUNTLET", "PARROT  ", "MACE    ", 
	"        ", "        ", 
	"CLOTHES ", "PADLOCK ", "POLICE  ", "OPERATOR", "FIREMEN ", 
	"TAXI    ", "        ", "WINE    ", "POISON  ", "WATER   ", 
	"MONKEY  ", "MIRROR  ", "DRAWING ", "CAVITY  ", "WOOD    ", 
	"RAGS    ", "CRATE   ", "COFFIN  ", "POINTS  ", "PICTURE ", 
	"PHONE   ", "HINGE   ", "RADIO   ", "LAMP    ", "WALL    ", 
	"TREE    ", "TROLL   ", "STATUE  ", "FIGURINE", "IDOL    ", 
	"BOULDERS", "ROCKS   ", "HOOK    ", "NEW     ", "OLD     ", 
	"SCROLL  ", "FIRE    ", "PLAQUE  ", "CURTAIN ", "DOOR    ", 
	"GATE    ", "QUEEN   ", "KING    ", "INDIANS ", "TABLE   ", 
	"TABLES  ", "SHOWER  ", "FIGURINE", "BULL    ", "BARS    ", 
	"DAGGER  ", "PRINTER ", "CASSETTE", "SILVER  ", "MANSION ", 
	"AND     ", "CURTAINS", "CRAZY   ", "BOULDER ", "        ",
	"        ", "        ", "        ", "        ", "        ",
	"        ", "        ", "        ", "        ", "        ",
	"        ", "        ", "        ", "        ", "        ",
	"        ", "        ", "        ", "        ", "        ",
	"        ", "        ", "        ", "        ", "        ",
	"WHERE   ", "WHAT    ", "WHO     ", "HOW     ", "WHY     ",
	"WHEN    ", "DAMN    ", "SHIT    ", "PISS    ", "CRAP    ",
	"FUCK    ", "FUCKER  "
};

int IVAL[] = { 
	0, 
	3,2,1,2,5,2,10,2,2,1,1,1,1,1,1,1,4,4,1,2,1,1,10,1,1,
	10,1,1,1,4,1,1,1,1,10,10,10,10,10,10,10,10,10,2,5,1,1,10,5,1,0,0
};

int IX, IR, IT, IBT;
int IC, ID, IE;		/* more general-purpose vars     */
int IPR[8];		/* more parameter-passing stuff  */
int IRIT;		/* parameter-passing             */
int IRB, IRX, IMZ[4], IXX;
int IRC, ITS, ISC;
int J1, J2;
double RND, R1, RND1;
int MMRLret;
int ISCP;
int ISCL;
int IHR, MIN;
int IDUMMY;
int IW63;



/*
 * ourtime()
 *
 * This is a rewrite of the original v17 Mystery Mansion OURTIME().  It returns
 * the current Time of Day in a six-double array.  
 */
void ourtime(ctod)
	double ctod[];
{
	time_t tloc;            /* unsigned int current TOD */
	struct tm *tmp;         /* time information pointer */

	/*
	 * Determine the current time of day.  This returns time_t representing
	 * the number of seconds since January 1, 1970 UTC.
	 */
	if (time(&tloc) == -1) {
		perror("time()");
			pak();
			exit(-1);
	}
	/* Massage that time into hours, mins, sec, etc */
	tmp = localtime(&tloc);
	ctod[2]  = (double)(tmp->tm_sec);         
	ctod[3]  = (double)(tmp->tm_min);         
	ctod[4]  = (double)(tmp->tm_hour);       

	return;
}


/*
 * pak() - Press A Key
 *
 * Goofy little routine to require a keypress before the game ends.  Useful
 * with win32-based boxes, since they close MS-DOS windows when the game
 * ends.
 */
void pak(void)
{
#ifndef UNIX
	printf("\n-- press a key to continue --"); 
	fflush(stdout);
	getchar();
#else
	return;
#endif
}
 

/*
void piwrd(iwrd)
	int iwrd[16][16];
{
	int i;
	char w0,w1,w2,w3,w4,w5,w6,w7;

	printf("          [01234567]\n");
	for (i=0; i<8; i++) {
	w0 = ((iwrd[i][0] >= ' ') && (iwrd[i][0] <= '~')) ? iwrd[i][0] : '*';
	w1 = ((iwrd[i][1] >= ' ') && (iwrd[i][1] <= '~')) ? iwrd[i][1] : '*';
	w2 = ((iwrd[i][2] >= ' ') && (iwrd[i][2] <= '~')) ? iwrd[i][2] : '*';
	w3 = ((iwrd[i][3] >= ' ') && (iwrd[i][3] <= '~')) ? iwrd[i][3] : '*';
	w4 = ((iwrd[i][4] >= ' ') && (iwrd[i][4] <= '~')) ? iwrd[i][4] : '*';
	w5 = ((iwrd[i][5] >= ' ') && (iwrd[i][5] <= '~')) ? iwrd[i][5] : '*';
	w6 = ((iwrd[i][6] >= ' ') && (iwrd[i][6] <= '~')) ? iwrd[i][6] : '*';
	w7 = ((iwrd[i][7] >= ' ') && (iwrd[i][7] <= '~')) ? iwrd[i][7] : '*';
		printf("IWRD[%02d]==[%c%c%c%c%c%c%c%c] %d %3d %d\n", i, 
			w0, w1, w2, w3, w4, w5, w6, w7, iwrd[i][8], 
			iwrd[i][9], iwrd[i][10]);
	}
	fflush(stdout);
}
*/

void MMRI(IV, IRS)
	int IV;
	int IRS;
{
	int J;

	IR=ITST[1];
	IC=0;
	for (J=1; J<=53; J++) {
		if ((IV == 1) && ((IVEN[J] > 10000) || 
		    ((IVEN[J]-100*(IVEN[J]/100)) != IR))) continue;
		if ((IV == 2) && (IVEN[J] < 20000)) continue;
		if ((IV == 4) && (IRS != J)) continue;
		if ((IV == 3) && ((IVEN[J] < 10000) || (IVEN[J] > 20000) ||
		    (IFSD(IVEN[J]) != IRS))) continue;
		if ((IV == 1) && (J == 4) && ((IVEN[4] == 493) ||
		    (IVEN[4] == 1493))) continue;
		if ((IV == 1) && (J == 5) && (IVEN[5] == 1316)) continue;
		if ((IV == 1) && (J == 17) && (IVEN[17] == 1141)) continue;
		if ((IV == 1) && (J == 19) && (IR == 16)) continue;
		if ((IV == 1) && (J == 29) && (IVEN[29] == 1214)) continue;
		if ((IV == 1) && (J == 30) && (IR == 25)) continue;
		if ((IV == 1) && (J == 37) && (IVEN[37] == 1153)) continue;
		if ((IV == 1) && (J == 39) && (IVEN[39] == 1253)) continue;
		if ((IV == 1) && (J == 40) && (IVEN[40] == 1151)) continue;
		if ((IV == 1) && (J == 42) && (IVEN[42] == 1151)) continue;
		if ((IV == 1) && (J == 46) && (IVEN[46] == 1101)) continue;
		if ((IV == 1) && (J == 28) && (IVEN[28] == 1184)) continue;
		if ((IV == 1) && (IC == 0)) printf("\n\nYOU CAN SEE:");
		if ((IV == 2) && (J == 53)) continue;
		if (J == 1) printf("\nTHE VAMPIRE RING ");
		if (J == 2) printf("\nA SET OF KEYS    ");
		if (J == 3) printf("\nA WIND UP CLOCK  ");
		if (J == 4) printf("\nA BATTERY LANTERN");
		if (J == 5) printf("\nA SILVER CROSS   ");
		if (J == 6) printf("\nA POCKET COMPASS ");
		if (J == 7) printf("\nA PIRATE TREASURE");
		if (J == 8) printf("\nA NEW BATTERY    ");
		if (J == 9) printf("\nA HAND GUN       ");
		if (J == 10) printf("\nA BUTCHER KNIFE  ");
		if (J == 11) printf("\nA TWO BLADED AXE ");
		if (J == 12) printf("\nA SWORD          ");
		if ((J == 13) && (ITST[10] != 13)) 
			printf("\nA VIAL OF POISON ");
		if ((J == 13) && (ITST[10] == 13)) 
			printf("\nAN EMPTY VIAL    ");
		if (J == 14) printf("\nA CLUB           ");
		if (J == 15) printf("\nA COIL OF ROPE   ");
		if (J == 16) printf("\nA DESK CHAIR     ");
		if (J == 17) printf("\nAN UNEVEN AMULET ");
		if (J == 18) printf("\nAN OLD TALISMAN  ");
		if ((J == 19) && (IFD(IVEN[19]) == 1)) 
			printf("\nA LIT CANDLE     ");
		if ((J == 19) && (IFD(IVEN[19]) != 1)) 
			printf("\nA CANDLE         ");
		if (J == 20) printf("\nA SILVER BULLET  ");
		if (J == 21) printf("\nA BOOK           ");
		if (J == 22) printf("\nA NOTE           ");
		if (J == 23) printf("\nA MATTER XMITTER ");
		if (J == 24) printf("\nSOME FOOD        ");
		if (J == 25) printf("\nA WITCH'S BROOM  ");
		if (J == 26) printf("\nA MATTER RECEIVER");
		if ((J == 27) && (IFD(IVEN[27]) == 1)) 
			printf("\nA BURNING TORCH  ");
		if ((J == 27) && (IFD(IVEN[27]) != 1)) 
			printf("\nA USELESS TORCH  ");
		if (J == 28) printf("\nA SMALL ROAD MAP ");
		if (J == 29) printf("\nA WOODEN WEDGE   ");
		if (J == 30) printf("\nA DUSTY GLOBE    ");
		if (J == 31) printf("\nA KITCHEN MATCH  ");
		if (J == 32) printf("\nA RUSTY SHOVEL   ");
		if (J == 33) printf("\nA SMALL OILCAN   ");
		if (J == 34) printf("\nA GREASY HATCHET ");
		if (J == 35) printf("\nA MYSTERY CHEST  ");
		if (J == 36) printf("\nA BAG OF PEARLS  ");
		if (J == 37) printf("\nA LARGE EMERALD  ");
		if (J == 38) printf("\nTWELVE GOLD COINS");
		if (J == 39) printf("\nA SILVER GOBLET  ");
		if (J == 40) printf("\nSEVERAL DIAMONDS ");
		if (J == 41) printf("\nSOME FINE JEWELRY");
		if (J == 42) printf("\nA RUBY NECKLACE  ");
		if (J == 43) printf("\nA JEWELED CROWN  ");
		if (J == 44) printf("\nSOME DARTS       ");
		if (J == 45) printf("\nA METAL SHIELD   ");
		if (J == 46) printf("\nA DUNGEON KEY    ");
		if (J == 47) printf("\nAN OLD BATTERY   ");
		if (J == 48) printf("\nA GAUDY GAUNTLET ");
		if (J == 49) printf("\nA PARANOID PARROT");
		if (J == 50) printf("\nA MENACING MACE  ");
		if (J == 51) printf("\nA BLANK          ");
		if (J == 52) printf("\nA BLANK          ");
		if (J == 53) printf("\nYOUR CLOTHES     ");
		IC=1;
	}
	if (((IV == 2) || (IV == 3)) && (IC == 0))
		printf("\nNOTHING");
	return;
}



int main(int argc, char **argv)
{
	int fd;
	int cin;
	int pwsp;
	int IP;
	int ic;
	int I, J, K=0, L;	/* general purpose variables     */
	int IF, IG=0;		/* and even more                 */
	int IM;			/* mystery number                */
	int INUM=0;		/* number of input-words         */
	int IANS;		/* for yes/no answers            */
	int ITR, IEF;	
	int IFC=0, IDN, IDE, IDS, IDW;
	int IW52, IW62, IW71;
	int ITST5;
	double TG;
	double R=0;
	double ITIM[6];		/* time for randomizing          */
	double tmp;
	double RNX;
	char IWP[8];		
	int IWRD[16][16];	/* input-word array              */
	char buf[92];		/* input buffers                 */
	char *p, *MSG;		/* string pointer for the parser */

	/*
	 * Initialization and startup
	 */
	setvbuf(stdout, (char*)NULL, _IONBF, 0);
	RND = 2;
MMSA:
	if (RND != 2) 
		goto MMSA_1000;

	ourtime(ITIM);
	RND      = (ITIM[2]*ITIM[3]+10)/3600.00;
	IM       = (int)(1000*RND);
	ITST[23] = IM;
	RND      = 0.001*IM;
	ITST[30] = ITST[30] + 3*ITST[29];

	printf("\nMystery Mansion was originally developed by Bill Wolpert "
	       "at the Naval\nUndersea Warfare Engineering Station (NUWES) "
	       "in Keyport, Washington, in\nFortran IV, circa 1981.  This "
	       "version was ported to C in April, 2000 and is \noccasionally "
	       "updated with bug fixes.  The latest source can be found at\n"
           "https://github.com/garnett1966/Mystery-Mansion.\n\n"
	       "                             - - -\n");

	printf("\nWELCOME TO MYSTERY MANSION. (REV.%d) MYSTERY #%d\n\n"
/*
 * Remove this old versioning information, since it's not really useful
 * anymore.
 *
   "THIS VERSION OF MYSTERY MANSION IS NOT COMPATIBLE WITH EARLIER VERSIONS\n"
   "MMFREEZE FILES.  PLEASE REPORT ANY PROBLEMS TO THE AUTHOR.\n"
 *
 */
   "THE ELEMENTS OF MYSTERY MANSION ARE BASED ON THE FACTS, FICTIONS AND\n"
   "FANTASIES OF THE PAST, PRESENT AND FUTURE. THE SCENARIO WAS DESIGNED\n"
   "TO CHALLENGE THE DARING AND YET, ENTERTAIN THE CURIOUS. ESCAPE FOR A\n"
   "FEW MOMENTS AND EXPERIENCE FRUSTRATION AND TRIUMPH, HOPE AND DESPAIR,\n"
   "POWER, LUST AND GREED. USING WHAT YOU KNOW, WHAT YOU CAN FIND OUT AND\n"
   "WHAT WORKS; TELL THE COMPUTER WHAT YOU WANT OR THINK YOU SHOULD DO.\n\n"
   "YOU ARE IN FRONT OF A HEAVY IRON GATE WHICH IS APPARENTLY THE ONLY\n"
   "WAY THROUGH A HIGH BRICK WALL PROTECTING AN OLD MANSION JUST VISIBLE\n"
   "THROUGH THE GATE. A ROAD LEADS TO THE EAST AND WEST ALONG THE WALL.\n"
   "IT IS DAWN AND A THIN LOW FOG IS JUST CLEARING FROM THE COOL AREAS.\n"
   "BEHIND YOU TO THE SOUTH ON THE OTHER SIDE OF THE ROAD IS A HIGHWAY\n"
   "GOING SOUTH AS FAR AS YOU CAN SEE. YOU CAN JUST SEE THE TAXI THAT\n"
   "DROPPED YOU OFF, DRIVING OUT OF SIGHT.\n", ITST[15], IM);

	ITST[18] = (int)(ITIM[4]*1200 + ITIM[3]*20 + ITIM[2]/3);
	ITST[19] = (int)(100*ITIM[4]+ITIM[3]);

MMSA_1000:
	RND = RND*7.7 - (int)(RND*7.7);
	IC  = (int)(RND*77)+1;
	if (((IC > 9) && (IC < 19)) || ((IC > 27) && (IC < 54))) 
		goto MMSA_1000;

	fflush(stdout);

	ITST[8] = IC;
	RND = RND*7.7 - (int)(RND*7.7);
	ID = (int)(RND*4);
	if (ID == 0) IE=7;
	if (ID == 1) IE=8;
	if (ID == 2) IE=9;
	if (ID == 3) IE=2;
	if ((IC < 28) && (ID == 0)) IE=3;
	if ((IC < 28) && (ID == 1)) IE=8;
	if ((IC < 28) && (ID == 2)) IE=14;
	if ((IC < 28) && (ID == 3)) IE=2;
	if ((IC < 10) && (ID == 0)) IE=3;
	if ((IC < 10) && (ID == 1)) IE=7;
	if ((IC < 10) && (ID == 2)) IE=9;
	if ((IC < 10) && (ID == 3)) IE=14;
	ITST[9]=IE;

	RND = RND*7.7 - (int)(RND*7.7);
	IF  = (int)(RND + 0.5);
	RND=RND*7.7 - (int)(RND*7.7);
	if ((IE == 3) && (IF == 0)) IG=(int)(RND*9)+1;
	if ((IE == 3) && (IF == 1)) IG=(int)(RND*9)+19;
	if ((IE == 7) && (IF == 0)) IG=(int)(RND*9)+1;
	if ((IE == 7) && (IF == 1)) IG=(int)(RND*24)+54;
	if ((IE == 8) && (IF == 0)) IG=(int)(RND*9)+19;
	if ((IE == 8) && (IF == 1)) IG=(int)(RND*24)+54;
	if ((IE == 9) && (IF == 0)) IG=(int)(RND*9)+1;
	if ((IE == 9) && (IF == 1)) IG=(int)(RND*24)+54;
	if ((IE == 14) && (IF == 0)) IG=(int)(RND*9)+1;
	if ((IE == 14) && (IF == 1)) IG=(int)(RND*9)+19;
	if ((IE == 2) && (IF == 0)) IG=(int)(RND*9)+19;
	if ((IE == 2) && (IF == 1)) IG=(int)(RND*24)+54;
	RND=RND*7.7 - (int)(RND*7.7);
	IVEN[(int)(RND*6)+9]=(IVEN[(int)(RND*6)+9]/100)*100+IG;

	ITST[10] = (int)(RND*6)+9;
	if ((int)(RND*6) == 0) ITST[11]=5;
	if (ITST[10] > 12) IRES[1]=10000+IC;
	if (ITST[10] > 12) goto MMSA_2000;
	RND=RND*7.7-(int)(RND*7.7);
	IF =(int)(RND+.5);
	RND=RND*7.7 - (int)(RND*7.7);
	if ((IE == 3) && (IF == 0)) IG=(int)(RND*9)+1;
	if ((IE == 3) && (IF == 1)) IG=(int)(RND*9)+19;
	if ((IE == 7) && (IF == 0)) IG=(int)(RND*9)+1;
	if ((IE == 7) && (IF == 1)) IG=(int)(RND*24)+54;
	if ((IE == 8) && (IF == 0)) IG=(int)(RND*9)+19;
	if ((IE == 8) && (IF == 1)) IG=(int)(RND*24)+54;
	if ((IE == 9) && (IF == 0)) IG=(int)(RND*9)+1;
	if ((IE == 9) && (IF == 1)) IG=(int)(RND*24)+54;
	if ((IE == 14) && (IF == 0)) IG=(int)(RND*9)+1;
	if ((IE == 14) && (IF == 1)) IG=(int)(RND*9)+19;
	if ((IE == 2) && (IF == 0)) IG=(int)(RND*9)+19;
	if ((IE == 2) && (IF == 1)) IG=(int)(RND*24)+54;
	IRES[1]=10000+IG;

MMSA_2000:
	IC=(int)(RND*3)+78;
	IXT[326]=75+100*IC;
	if (IC == 79) IXT[266]=7579;
	IC=IC+1;
	if (IC == 81) IC=78;
	IXT[327]=76+100*IC;
	if (IC == 79) IXT[266]=7679;
	IC=IC+1;
	if (IC == 81) IC=78;
	IXT[328]=77+100*IC;
	if (IC == 79) IXT[266]=7779;
	RND=RND*7.7-(int)(RND*7.7);
	IVEN[8]=200+(int)(RND*6+29);
	
	/* Set up Mole */
	RND=RND*7.7-(int)(RND*7.7);
	IRES[4]=(int)(RND*9+43);

	/* Set up reversible letters */
	IWRD[1][1]='M';
	IWRD[1][2]='I';
	IWRD[1][3]='W';
	IWRD[1][4]='O';
	IWRD[1][5]='X';
	IWRD[2][1]='M';
	IWRD[2][2]='I';
	IWRD[2][3]='W';
	IWRD[2][4]='O';
	IWRD[2][5]='H';
		
	/*
	 * Juggle letters randomly
	 */
	for (I=1; I<=2; I++) {
		RND=RND*7.7-(int)(RND*7.7);
		for (J=1; J<=(int)(RND*10)+5; J++) {
			RND=RND*7.7-(int)(RND*7.7);
			if (RND < 0.5) goto MMSA_3100;
			IC = IWRD[I][1];
			IWRD[I][1]=IWRD[I][3];	/* flip M/W */
			IWRD[I][3]=IC;
MMSA_3100:
			RND=RND*7.7-(int)(RND*7.7);
			if (RND < 0.5) goto MMSA_3200;
			IC = IWRD[I][2];
			IWRD[I][2]=IWRD[I][4];	/* flip I/O */
			IWRD[I][4]=IC;
MMSA_3200:
			RND=RND*7.7-(int)(RND*7.7);
			if (RND < 0.5) continue;
			IC = IWRD[1][5];	/* flip X/H */
			IWRD[1][5]=IWRD[2][5];
			IWRD[2][5]=IC;
		}
	}

/* MMSA_3300: */
	RND=RND*7.7-(int)(RND*7.7);
	if (RND < 0.5) goto MMSA_3400;
	IC = IWRD[1][1];
	IWRD[1][1]=IWRD[1][5];
	IWRD[1][5]=IC;

MMSA_3400:
	RND=RND*7.7-(int)(RND*7.7);
	if (RND < 0.5) goto MMSA_3500;
	IC = IWRD[2][1];
	IWRD[2][1]=IWRD[2][5];
	IWRD[2][5]=IC;

MMSA_3500:
	for (I=1; I<=2; I++) {
		IVRB[4*I][0] = IWRD[I][1];
		IVRB[4*I][1] = IWRD[I][2];
		IVRB[4*I][2] = IWRD[I][3];
		IVRB[4*I][3] = IWRD[I][4];
		IVRB[4*I][4] = IWRD[I][5];

		IVRB[4*I][0] = IWRD[I][5];
		IVRB[4*I][1] = IWRD[I][4];
		IVRB[4*I][2] = IWRD[I][3];
		IVRB[4*I][3] = IWRD[I][2];
		IVRB[4*I][4] = IWRD[I][1];

		if ((IWRD[I][1] != 'W') && (IWRD[I][1] != 'M'))
			goto MMSA_3600;

		IVRB[4*I+2][0] = IWRD[I][3];
		IVRB[4*I+2][1] = IWRD[I][2];
		IVRB[4*I+2][2] = IWRD[I][1];
		IVRB[4*I+2][3] = IWRD[I][4];
		IVRB[4*I+2][4] = IWRD[I][5];

		IVRB[4*I+3][0] = IWRD[I][5];
		IVRB[4*I+3][1] = IWRD[I][4];
		IVRB[4*I+3][2] = IWRD[I][1];
		IVRB[4*I+3][3] = IWRD[I][2];
		IVRB[4*I+3][4] = IWRD[I][3];
		continue;
MMSA_3600:
		IVRB[4*I+2][0] = IWRD[I][1];
		IVRB[4*I+2][1] = IWRD[I][2];
		IVRB[4*I+2][2] = IWRD[I][5];
		IVRB[4*I+2][3] = IWRD[I][4];
		IVRB[4*I+2][4] = IWRD[I][3];

		IVRB[4*I+3][0] = IWRD[I][3];
		IVRB[4*I+3][1] = IWRD[I][4];
		IVRB[4*I+3][2] = IWRD[I][5];
		IVRB[4*I+3][3] = IWRD[I][2];
		IVRB[4*I+3][4] = IWRD[I][1];
	}

	/*
	 * Set the string-version of the weapon for later output usage.
	 */
	if (ITST[10] == 9)  strcpy(IWP, "GUN");
	if (ITST[10] == 10) strcpy(IWP, "KNIFE");
	if (ITST[10] == 11) strcpy(IWP, "AXE");
	if (ITST[10] == 12) strcpy(IWP, "SWORD");
	if (ITST[10] == 13) strcpy(IWP, "VIAL");
	if (ITST[10] == 14) strcpy(IWP, "CLUB");

MMSA_3800:
	RND=RND*7.7-(int)(RND*7.7);
	IC = (int)(RND*6 + 2);
	if (IC == 4) IC=8;
	if (IC == 5) IC=9;
	if (IC == 6) IC=14;
	if (IC == ITST[9]) goto MMSA_3800;
	ITST[39]=IC;

MMSB:
	putchar('\n');
	for (I=0; I<16; I++) 
		for (J=1; J<16; J++) 
			IWRD[I][J] = ' ';
	memset(buf, ' ', sizeof(buf));
	putchar('\n');
	fflush(stdout);

	/*
	 * This stuff is not in the original, but it's needed for the C vers.
	 * First point to the 1st element of the input buffer, 
	 * so that input will start from there.
	 */
	cin  = 0;
	pwsp = 0;
	p    = buf;
	/*
	 * Now loop, reading char-by-char input.  This is not very elegant,
	 * but it works well with the old mystery-mansion input paradigm.
	 * We upshift characters as they come in.
	 */
	while ((*p=fgetc(stdin)) != EOF) {
		if (*p == '\n')
			break;
		if (isspace(*p) && !pwsp)
			continue;
		pwsp = 1;
		if ((*p >= 'a') && (*p <= 'z')) *p -= ' ';
		if (++cin == 72)
			break;
		p++;
	} 
	/* 
	 * Catch EOF from the input, treat it like an attempt to quit. Is 
	 * this wise? 
	 */
	if (feof(stdin)) {
		clearerr(stdin);
		goto MMSF_1021;
	}
	MSG = buf;
	/*
	 * Transform the newline into a space 
	 */
	*p = ' ';


	/*
	 * Back to the port.  This loop separates words into IWRD
	 */
	for (IC=-1, INUM=0; INUM < 8; INUM++) {
		for (J=0; J < 9; J++) {
			IC++;
			IWRD[INUM][J] = MSG[IC];
			/* dblspace == end of input */
			if ((MSG[IC] == ' ') && (MSG[IC+1] == ' '))
				goto MMSB_80;
			/* space == end of word */
			if (MSG[IC] == ' ') 
				break;
		}
		/* more than 8 chars == word is too long */
		if (J == 9) {
			IPR[2] = 150;
			IPR[3] = 0;
			goto MMSD;
		}
	}
	/* more than 8 words == too many words */
	IPR[2] = 170;
	IPR[3] = 0;
	goto MMSD;

MMSB_80:
	for (I=8; I<=15; I++)
		for (J=0; J<8; J++)
			IWRD[J][I]=0;

	INUM++;
	for (I=0; I<INUM; I++) {
		for (K=1; K<=89; K++) {
			IC = 0;
			for (J=0; J<8; J++) {
				if (IWRD[I][J] == IVRB[K][J])
					IC++;
				if (IC == 8)
					IWRD[I][8]=1;
				if (IC == 8)
					goto MMSB_169;
			}
		}
	
		for (K=1; K<=11; K++) {
			IC=0;
			for (J=0; J<8; J++) {
				if (IWRD[I][J] == IDTN[K][J])
					IC++;
				if (IC == 8)
					IWRD[I][8]=2;
				if (IC == 8)
					goto MMSB_169;
			}
		}

		for (K=1; K<=149; K++) {
			IC=0;	
			for (J=0; J<8; J++) {
				if (IWRD[I][J] == ITEM[K][J])
					IC++;
					if (IC == 8)
						IWRD[I][8]=3;
					if (IC == 8)
						goto MMSB_169;
				}
		}
	
		for (K=1; K<=17; K++) {
			IC=0;
			for (J=0; J<8; J++) {
				if (IWRD[I][J] == IRSN[K][J])
					IC++;
				if (IC == 8)
					IWRD[I][8]=4;
				if (IC == 8)
					goto MMSB_169;
			}
		}
	
		for (K=1; K<=17; K++) {
			IC=0;
			for (J=0; J<8; J++) {
				if (IWRD[I][J] == IPRP[K][J])
					IC++;
				if (IC == 8)
					IWRD[I][8]=5;
				if (IC == 8)
					goto MMSB_169;
			}
		}
		if (I > 1)
			goto MMSB_168;
		IC=IWRD[0][0];
		ic=IWRD[0][1];
		ID=0;
		if ((IC == 'N') && (ic == ' ')) ID=1;
		if ((IC == 'E') && (ic == ' ')) ID=2;
		if ((IC == 'S') && (ic == ' ')) ID=3;
		if ((IC == 'W') && (ic == ' ')) ID=4;
		if ((IC == 'U') && (ic == ' ')) ID=5;
		if ((IC == 'D') && (ic == ' ')) ID=6;
		if ((IC == 'B') && (ic == ' ')) ID=7;
		if ((IC == 'L') && (ic == ' ')) ID=8;
		if ((IC == 'F') && (ic == ' ')) ID=9;
		if ((IC == 'R') && (ic == ' ')) ID=10;
		if (ID != 0) IWRD[0][8] = 1;
		if (ID != 0) IWRD[1][8] = 2;
		if (ID != 0) INUM=2;
		if (ID != 0) IWRD[0][9] = 1;
		if (ID != 0) IWRD[1][9] = ID;
		if (ID != 0) goto MMSB_171;
MMSB_168:
		IPR[2] = 160;
		IPR[3] = I;
		goto MMSD;
MMSB_169:
		IWRD[I][9]=K;
		if (K > 143) {
			IPR[2] = 165;
			IPR[3] = 0;
			goto MMSD;
		}
		if ((K > 137) && (K < 144)) {
			IPR[2] = 166;
			IPR[3] = 0;
			goto MMSD;
		}
	}

	IX = 0;
	if ((IWRD[0][8]==5) && (IWRD[0][9]==11) && (INUM==1)) IX=9999;
	if (IX==9999) IWRD[0][8]=1;
	if (IX==9999) IWRD[0][9]=28;
	if (IWRD[0][8] != 1) {
		IPR[2] = 180;
		IPR[3] = 0;
		goto MMSD;
	}
	
	if ((IWRD[0][9] == 19) && (IWRD[1][9] == 19))
		goto MMSB_9999;
	if (((IWRD[1][8] == 1) || (IWRD[2][8]==1) || (IWRD[3][8] == 1) ||
	     (IWRD[4][8] == 1)) && (IWRD[0][8] != 51)) {
		IPR[2] = 190;
		IPR[3] = 0;
		goto MMSD;
	}
	for (I=1; I<8; I++) {
		if ((IWRD[I][8]==5) && ((IWRD[I][9]==9) || (IWRD[I][9]==12)))
			goto MMSB_172;
		if ((IWRD[I][8]==2) && (IWRD[I-1][9]==74) && (IWRD[I][9]==5))
			IWRD[I-1][9]=21;
		if ((IWRD[I][8]==2) && (IWRD[I-1][9]==21) && (IWRD[I][9]==5))
			goto MMSB_172;
		if ((IWRD[I][8]==2) && (IWRD[I-1][9]==73) && (IWRD[I][9]==6))
			IWRD[I-1][9]=23;
		if ((IWRD[I][8]==2) && (IWRD[I-1][9]==23) && (IWRD[I][9]==6))
			goto MMSB_172;
		if ((IWRD[I][8] != 3) || (IWRD[I+1][8] != 3)) continue;
		if ((IWRD[I][9] == 86) && (IWRD[I+1][9] == 8)) goto MMSB_172;
		if ((IWRD[I][9] == 8) && (IWRD[I+1][9] == 4)) goto MMSB_172;
		if ((IWRD[I][9] == 106) && (IWRD[I+1][9] == 20)) goto MMSB_172;
		if ((IWRD[I][9] == 106) && (IWRD[I+1][9] == 5)) goto MMSB_172;
		if ((IWRD[I][9] == 106) && (IWRD[I+1][9] == 39)) goto MMSB_172;
		if ((IWRD[I][9] == 87) && (IWRD[I+1][9] == 8)) 
			IWRD[I+1][9]=47;
		if ((IWRD[I][9] == 87) && (IWRD[I+1][9] == 47)) goto MMSB_172;
		continue;
MMSB_172:
		/*
		 * This word deletion- and shifting-code fixed 1/18/2000;
		 * this section is seriously kludged from the original due
		 * to the strangeness of Fortran IV.  Note that the behavior
		 * is not identical to the original because of the way we
		 * have replaced the use of spaces with nulls; this shouldn't
		 * be a problem unless someone gets pedantic about it.
		 */
		INUM -= 1;
		for (J=I; J<8; J++) {
			for (K=0; K<10; K++) {
				IWRD[J][K] = IWRD[J+1][K];
				if (J == 7) IWRD[J+1][K]=0;
			}	
		}
		I -= 1;
	}

	if ((IWRD[0][9]==41) && ((INUM == 1) || (IWRD[1][8]==2)))
		IWRD[0][9]=1;
	if (IWRD[0][9] == 72)
		IWRD[0][9]=1;
	if (IWRD[0][9] == 73)
		IWRD[0][9]=60;
	if ((IWRD[1][8] == 3) && (IWRD[1][9]==61)) IWRD[1][9]=13;
	if ((IWRD[1][8] == 3) && (IWRD[1][9]==109)) IWRD[1][9]=91;
	if ((IWRD[2][8] == 3) && (IWRD[2][9]==109)) IWRD[2][9]=91;
	if ((IWRD[1][8] == 3) && (IWRD[1][9]==111)) IWRD[1][9]=83;
	if ((IWRD[2][8] == 3) && (IWRD[2][9]==111)) IWRD[2][9]=83;
	if (IWRD[2][9]==97) INUM -= 1;
	if ((IWRD[INUM-1][9]==108) && (IWRD[0][9]==21)) INUM -= 2;
	if ((IWRD[INUM-1][9]==108) && (IWRD[0][9]==23)) INUM -= 2;

MMSB_171:
	IR = ITST[1];

	if (IWRD[0][0] != ' ') ITST[22] = 0;
	ITST[27] = ITST[5];
	ITST[5] += 1;
	RND=RND*7.7-(int)(RND*7.7);
	if ((RND <= 0.0) || (RND >= 1.0)) RND = 0.5;
	R = RND;
	ourtime(ITIM);
	IC = (int)(ITIM[4]*1200 + ITIM[3]*20 + ITIM[2]/3);
	if ((IC - ITST[18]) < 0) ITST[18] = 0;
	if (ITST[5] > 1) ITST[5] = ITST[5] + (IC - ITST[18])/10;
	ITST5 = ITST[5];
	if (((IC - ITST[18]) >= 10) && (IFSD(ITST[5]) == 95)) 
		printf("\nYOU ARE TAKING TOO LONG.\n");
	ITST[18] = IC;
	if ((ITST[1] == 46) && ((IWRD[0][9] != 1) || (IWRD[1][9] != 11) ||
	    (IWRD[1][8] != 2)) && (IXT[320] == 0)) {
		IPR[2] = 176;
		IPR[3] = 0;
		goto MMSD;
	}
	IT = 0;
	if (ITST[32] != 0) IT = ITST[5] - ITST[32];
	if (IT > 15) IXT[352] = 4651;
	if (IT > 15) IXT[351] = 4649;
	if ((IT > 24) && (IXT[319] != 0)) IXT[319] = 4652;
	if (IT > 24) IXT[353] = 4650;
	if (IT > 36) IXT[354] = 4653;
	if (IT > 36) IXT[318] = 4645;
	IC = 0;
	tmp = (double)ITST5;
	tmp /= 25.00;
	if ((tmp - floor(tmp)) == 0) IC = ITST5/25 + 6;
MMSB_174:
	if (IC > 12) IC -= 12;
	if (IC > 12) goto MMSB_174;
	if ((IC != 0) && ((IR < 28) || (IR > 53))) 
		printf("\n\nTHE TOWER BELL RANG %d TIMES\n", IC);
	IC = ITST5 - (ITST5/25)*25;
	if ((IC == 13) && (IWRD[0][9] != 20) && (IWRD[1][9] != 40))
		ITST[3] -= 2;
	if ((IC == 12) && (IR < 28) && (ITST5 < 450))
		printf("\n\nYOU HEARD A WOMAN SCREAM\n");
	if ((IC == 12) && (IR > 53) && (ITST5 >= 300) && (ITST5 < 450))
		printf("\n\nYOU HEARD A WOLF HOWL\n");
	if ((IC == 12) && (IR > 53) && (ITST5 < 300))
		printf("\n\nYOU HEARD A CROW CAW AS IT FLEW BY\n");
	if ((IC == 12) && (IR < 54) && (IR > 27) && (IR != 53) && (ITST5 < 450))
		printf("\n\nYOU HEARD SOME ROCKS FALLING NEARBY\n");
	if (ITST[5] > 300) IXT[27] = 0;
	if (IVEN[73] != 11) goto MMSB_194;
	if ((IVEN[22] > 20000) && (IC == 12)) IVEN[73]=122;
	if ((IVEN[4] > 20000)  && (IVEN[6] > 20000) && (ITST5 == 120))
		IVEN[73]=222;
	if ((ITST[8] == 0) && (IC == 5) && (IRES[ITST[9]] != 0)) IVEN[73]=322;
	if ((ITD(IVEN[4]) == 4) && (IR == 11)) IVEN[73]=422;
MMSB_194:
	if ((IFSD(IVEN[73]) == 22) && (IR > 9) && (IR < 19) && 
	    (IWRD[0][9] != 42) && (IWRD[0][9] != 40))
		printf("\n\nYOU HEARD A PHONE RING\n");
	if (IVEN[73] != 11) IVEN[73] += 1000;
	if (IVEN[73] >= 7000) IVEN[73]=11;
	if ((ITST5 >= 450) && (IXT[26] != 0)) {
		IPR[2] = 185;
		IPR[3] = 0;
		goto MMSD;
	}
	if ((IR > 18) && (IR < 28) && (ITST5 > 490))
		printf("\n\nTHE SMOKE IS MAKING YOU CHOKE\n");
	if ((IR > 18) && (IR < 28) && (ITST5 >= 500)) {
		IPR[2] = 189;
		IPR[3] = 0;
		goto MMSD;
	}
	if ((IR == 93) && (ITST5 >= 451) && (IXT[26] == 0)) {
		IPR[2] = 191;
		IPR[3] = 0;
		goto MMSD;
	}
	if (ITST5 >= 550) {
		IPR[2] = 192;
		IPR[3] = 0;
		goto MMSD;
	}
	R = RN(R);
	if ((IVEN[8] == 0) && (IVEN[47] > 227) && (IVEN[47] < 235) &&
	    (ITST[1] < 28)) IVEN[47] = 0;
	if ((IVEN[8] == 0) && (IVEN[47] == 0)) IVEN[8] = 229 + (int)(R*6);
	if (IFD(IVEN[4]) == 0) goto MMSB_210;
	ITST[6] += 1;
	IBT = ITST[6] - (ITST[6]/1000)*1000;
	if (IBT < 100) goto MMSB_210;
	if (IBT == 120) IVEN[4] -= 1000;
	if (IVEN[4] < 20000) goto MMSB_210;
	if (IVEN[8] < 20000) goto MMSB_204;
	IVEN[8] = 0;
	ITST[6] = (ITST[6]/1000)*1000;
	printf("\nI REPLACED YOUR WEAK LANTERN BATTERY\n");
	IVEN[47] = 20200+IR;
	goto MMSB_210;
MMSB_204:
	IBT -= 100;
	if (IBT == 20) {
		printf("\nYOUR LANTERN IS OUT\n");
		goto MMSB_210;
	}
	tmp = ((double)(IBT))/5.0;
	if (tmp == floor(tmp)) 
		printf("\nTHE BATTERY IN YOUR LANTERN IS GETTING WEAK\n");
MMSB_210:
	if (IFD(IVEN[19] == 0)) goto MMSB_220;
	ITST[6] += 1000;
	IBT = ITST[6]/1000;
	if (IBT < 20) goto MMSB_220;
	if ((IBT == 20) && (IVEN[19] == 1216)) ITST[6] -= 20000;
	if (IBT == 32) ITST[6] -= 32000;
	if ((IBT == 29) && (IVEN[19] > 20000)) 
		printf("\nYOUR CANDLE IS BURNING LOW\n");
	if ((IBT == 32) && (IVEN[19] > 20000)) 
		printf("\nYOUR CANDLE BURNED OUT\n");
	if (IBT == 32) IVEN[19] = 1216;
MMSB_220:
	if (IFD(IVEN[31]) == 0) goto MMSB_230;
	ITST[7] += 10000;
	IBT=ITST[7]/10000;
	if (IBT < 3) goto MMSB_230;
	ITST[7] -= 30000;
	if (IVEN[31] > 20000)
		printf("\nYOUR MATCH BURNED OUT\n");
	IVEN[31] = 118;
MMSB_230:
	if ((IVEN[27] == 0) && (IR == 53) && (IXT[320] != 0)) IVEN[27]=1242;
	if ((IVEN[27] == 0) && (IR == 42) && (IXT[320] == 0)) IVEN[27]=1245;
	if ((IVEN[27] == 0) && ((IR == 28) || (IR == 33))) IVEN[27]=1230;
	if ((IVEN[27] != 0) && (IVEN[27] < 20000) && (IR < 9) &&
	    (IFSD(IVEN[27]) > 27) && (IFSD(IVEN[27]) < 54)) IVEN[27]=0;
	if (IVEN[27] == 0) ITST[7] = ITST[7] - 100*ITFD(ITST[7]);
	if (IFD(IVEN[27]) == 0) goto MMSB_250;
	ITST[7] += 100;
	IBT = ITFD(ITST[7]);
	if (((IR < 28) || (IR > 53)) && (IVEN[27] > 21000))
		printf("\nYOUR TORCH BURNED OUT\n");
	if ((((IR < 28) || (IR > 53)) && (IFD(IVEN[27]) == 1)))
		IVEN[27] -= 1000;
	if (IBT < 21) goto MMSB_250;
	if ((IBT < 25) && (IVEN[27] > 21000))
		printf("\nYOUR TORCH IS FLICKERING AND GETTING DIM\n");
	if ((IBT == 25) && (IVEN[27] > 21000))
		printf("\nYOUR TORCH BURNED OUT\n");
	if ((IBT == 25) && (IFD(IVEN[27]) == 1)) IVEN[27] -= 1000;
MMSB_250:
	IXT[16] = (IXT[16]/10000)*10000+2724;
	IXT[57]=0;
	IVEN[68]=27;
	IC=ITST5-(ITST5/25)*25;
	if (IC > 21) IXT[16] = (IXT[16]/10000)*10000+8824;
	if (IC < 1) IXT[57]=8988;
	if (IC > 21) IVEN[68]=88;
	IXT[271]=0;
	if (IVEN[16] == 519) IXT[271]=8719;
	for (I=1; I<=17; I++) {
		RND=RND*7.7-(int)(RND*7.7);
		if ((IFD(IRES[I]) == 1) && (RND < 0.3)) IRES[I] -= 1000;
		if ((IFD(IRES[I]) == 2) && (RND < 0.3)) IRES[I] -= 1000;
		if ((IFD(IRES[I]) == 2) && (RND > 0.7)) IRES[I] += 1000;
		if ((IFD(IRES[I]) == 9) && (RND < 0.3)) IRES[I] += 1000;
		if ((IFD(IRES[I]) == 8) && (RND < 0.3)) IRES[I] += 1000;
		if ((IFD(IRES[I]) == 8) && (RND > 0.7)) IRES[I] -= 1000;
	}
	if ((ITST[31] == 1) && (RND < 0.3)) ITST[31]=0;
	if ((ITST[31] == 2) && (RND < 0.3)) ITST[31]=1;
	if ((ITST[31] == 2) && (RND > 0.7)) ITST[31]=3;
	if ((ITST[8] == 0) && (IR == IFSD(IRES[ITST[9]])) &&
	    (IRES[ITST[9]] < 10000) && ((IVEN[10] == 10200+ITST[9]) ||
	    (IVEN[11] == 10300+ITST[9]) || (IVEN[12] == 10300+ITST[9])) &&
	    ((ITST5-1) != ITST[13])) {
		IPR[2] = 251;
		IPR[3] = 0;
		goto MMSD;
	}
	if ((IFSD(IRES[13]) != IR) || (ITST[13] == (ITST5-1)) ||
	    (IRES[13] > 10000)) goto MMSB_253;
	R=RN(R);
	if ((R < 0.8) && (IVEN[34] == 17213))
		printf("\nTHE DWARF THREW A HATCHET AT YOU AND MISSED. HE "
			"CURSED, RAN AROUND\nYOU AND PICKED IT UP\n");
	if ((R > 0.8) && (IVEN[34] == 17213)) {
		IPR[2] = 254;
		IPR[3] = 0;
		goto MMSD;
	}
/*
 * IWRD transformations can stop here
 */
MMSB_253:
	if ((IR == 92) && (IRES[16] == 0)) IRES[16]=995;
	R=RN(R);
	if ((IFSD(IRES[16]) != IR) || (IRES[16] > 1999) || 
	    ((IWRD[0][9] != 22) && (R > 0.33))) goto MMSB_260;
	IWRD[0][10] = 2;
	IWRD[0][9] = 46;
	IWRD[1][0] = 'W';
	IWRD[1][1] = 'A';
	IWRD[1][2] = 'R';
	IWRD[1][3] = 'R';
	IWRD[1][4] = 'I';
	IWRD[1][5] = 'O';
	IWRD[1][6] = 'R';
	IWRD[1][8] = 4;
	IWRD[1][9] = 16;
	printf("\nTHE WARRIOR JUST GAVE A FEARSOME YELL AND ATTACKED.\n");
	IPR[2] = 0;
	IPR[3] = 0;
	goto MMSH;
MMSB_260:
	/*
	 * Vampire Routine
	 */
	if ((IFSD(IRES[5]) == IR) && (IVEN[5] < 20000) &&
	    (ITST[13] != (ITST5-1))) {
		IPR[2] = 262;
		IPR[3] = 0;
		goto MMSD;
	}
	/* If the vampire's not awake, and we're two hours into gametime, */
	/* then put him in the creepy crypt */
	if ((IRES[5] == 300) && (ITST5 >= 50)) IRES[5]=5;

	/*
	 * Elf Routine
	 */
	if ((IFSD(IRES[11]) != IR) || (ITST[13] == (ITST5-1)) ||
	    (IRES[11] > 10000)) goto MMSB_270;
	R=RN(R);
	IC=0;
	for (J=(int)R*52+1; J<=52; J++) {
		if (IVEN[J] > 20000) IC=J;
		if (IC != 0) goto MMSB_264;
	}
	for (J=1; J<=(int)R*52+1; J++) {
		if (IVEN[J] > 20000) IC=J;
		if (IC != 0) goto MMSB_264;
	}
	goto MMSB_266;
MMSB_264:
	for (J=1; J<=52; J++) {
		if (((IVEN[J]/10000) == 1) && (IFSD(IVEN[J]) == 11))
			IVEN[J] = IVEN[J]-10011+79;
	}
MMSB_266:
	if (IC != 0) IVEN[IC]=IVEN[IC]-10000+11;
	if (IR < 62) IRES[11]=870;
	if (IR > 61) IRES[11]=854;
	if (IC == 0) printf("\nTHE ELF KICKED YOU IN THE SHIN AND RAN OFF\n");
	if (IC != 0) {
		printf("\nTHE ELF RAN OFF AFTER STEALING YOUR ");
		for (L=0; L<8; L++)
			putchar(ITEM[IC][L]);
		putchar('\n');
	}
MMSB_270:
	if ((IRES[ITST[39]] > 10000) || (IRES[ITST[39]] == 0) ||
	    (ITST[5] < 300)) goto MMSB_280;
	if ((IROM[IFSD(IRES[ITST[39]])] < 20000) && 
	    (IFSD(IRES[ITST[39]]) < 54)) goto MMSB_280;
	IRES[10]=IRES[ITST[39]];
	IRES[ITST[39]]=0;
	if (IR == IFSD(IRES[10])) {
		printf("\nIN A MATTER OF SECONDS AND BEFORE YOU CAN DO "
			"ANYTHING, THE ");
		for (L=0; L<8; L++) 
			putchar(IRSN[ITST[39]][L]);
		printf("\nGROWS HAIR, FANGS AND CLAWS GROTESQUELY "
			"AND TRANSFORMS INTO A WEREWOLF.\n");
	}
MMSB_280:
	if ((IR == IFSD(IRES[10])) && ((ITST5-1) != ITST[13]) &&
	    (IRES[10] < 10000)) {
		IPR[2] = 285;
		IPR[3] = 0;
		goto MMSD;
	}
	R=RN(R);
	IC=(int)(R*3);
	R=RN(R);
	ID=(int)(R*8+2);
	if ((ITD(IVEN[35]) == 5) && (IC != 0) && (IWRD[0][9] == 1)) {
		IPR[2]=287;
		IPR[3]=ID;
		goto MMSD;
	}
	if ((IR == 52) && (IFSD(IRES[4]) == 52)) IXT[258]=0;	
	if ((IVEN[53] > 20000) || (IR == 1) || (IR == 20) || (IR == 25))
		goto MMSB_295;
	for (I=2; I<17; I++) {
		if ((I == 4) || (I == 15) || (IFSD(IRES[I]) != IR))
			continue;
		printf("\nYOU NOTICED THAT YOU WERE BEING LAUGHED AT BY THE ");
		for (L=0; L<8; L++)
			putchar(IRSN[I][L]);
		putchar('\n');
		goto MMSB_295;
	}
MMSB_295:
	if ((IWRD[0][9] == 21) || (IWRD[0][9] == 22) || 
	    (IWRD[0][9] == 24) || (IWRD[0][9] == 25) ||
	    (IWRD[0][9] == 26) || (IWRD[0][9] == 27)) {
		MMRLret=0;
		goto MMRL;
	}
RLret0:
	
	IWRD[0][10] = INUM;
	if ((INUM == 1) && (IWRD[0][9] == 25)) goto MMSC;
	if (IWRD[0][9] < 18) goto MMSC;
	if ((IWRD[0][9] > 17) && (IWRD[0][9] < 36)) goto MMSF;
	if ((IWRD[0][9] > 35) && (IWRD[0][9] < 54)) goto MMSH;
	if (IWRD[0][9] > 53) goto MMSJ;

MMSB_9999:
	IPR[2]=11111;
	IPR[3]=0;
	goto MMSD;

MMSC:
	R=RND;
	if ((ITST[5] > 300) && (IWRD[0][9] > 3) && (IWRD[0][9] < 12)) {
		IPR[2]=290;
		IPR[3]=0;
		goto MMSD;
	}
	if ((IWRD[0][8] == 1) && (IWRD[0][9] == 25)) goto MMSC_1142;
	if (IWRD[0][9] == 1) goto MMSC_1000;
	if (IWRD[0][9] == 2) goto MMSC_300;
	if (IWRD[0][9] == 3) goto MMSC_400;
	if (IWRD[0][9] == 4) goto MMSC_600;
	if (IWRD[0][9] == 5) goto MMSC_500;
	if (IWRD[0][9] == 6) goto MMSC_700;
	if (IWRD[0][9] == 7) goto MMSC_600;
	if (IWRD[0][9] == 8) goto MMSC_600;
	if (IWRD[0][9] == 9) goto MMSC_500;
	if (IWRD[0][9] == 10) goto MMSC_700;
	if (IWRD[0][9] == 11) goto MMSC_600;
	if (IWRD[0][9] == 12) goto MMSC_900;
	if (IWRD[0][9] == 13) goto MMSC_970;
	if (IWRD[0][9] == 14) goto MMSC_950;

MMSC_300:
	if (IWRD[0][10] != 1) goto MMSC_310;
	IWRD[0][10] = 2;
	IWRD[1][8] = 2;
	IWRD[1][9] = 2;
	goto MMSC_1000;
MMSC_310:
	if (IWRD[0][10] == 2) goto MMSC_330;
	if ((IWRD[0][10] > 3) || (IWRD[1][8] != 5) || (IWRD[1][9] != 5)) {
		IPR[2]=320;
		IPR[3]=0;
		goto MMSD;
	}
	IWRD[1][8] = IWRD[2][8];
	IWRD[1][9] = IWRD[2][9];
MMSC_330:
	if (IWRD[1][8] != 3) {
		IPR[2]=320;
		IPR[3]=0;
		goto MMSD;
	}
	if ((IVEN[15] < 20000) || (IWRD[1][9] != 77) || 
	    ((IR != 93) && (IR != 94) && (IR != 95) && (IR != 96) &&
	     (IR != 70) && (IR != 66) && (IR != 67) && (IR != 92))) 
		goto MMSC_335;
	IRB=ITST[1];
	ITST[14]=IRB;
	if (ITST[1] == 93) IR=92;
	if (ITST[1] == 94) IR=66;
	if (ITST[1] == 95) IR=70;
	if (ITST[1] == 96) IR=67;
	if (ITST[1] == 67) IR=96;
	if (ITST[1] == 70) IR=95;
	if (ITST[1] == 66) IR=94;
	if (ITST[1] == 92) IR=93;
	ITST[1]=IR;
	goto MMSC_1141;
MMSC_335:
	if ((IWRD[1][9] == 77) && 
	    ((IR == 66) || (IR == 67) || (IR == 70) || (IR == 92) ||
	     (IR == 93) || (IR == 94) || (IR == 95) || (IR == 96))) {
		IPR[2]=340;
		IPR[3]=0;
		goto MMSD;
	}
	
	if (IWRD[1][9] == 77) {
		IPR[2]=350;
		IPR[3]=0;
		goto MMSD;
	}
	if ((IWRD[1][9] == 78) && (IR != 97) && (IR != 98)) {
		IPR[2]=360;
		IPR[3]=0;
		goto MMSD;
	}
	if (IWRD[1][9] == 78) {
		IPR[2]=370;
		IPR[3]=0;
		goto MMSD;
	}
	if ((IWRD[1][9] == 93) && (IR != 93)) {
		IPR[2]=390;
		IPR[3]=0;
		goto MMSD;
	}
	if (IWRD[1][9] == 93) {
		IPR[2]=385;
		IPR[3]=0;
		goto MMSD;
	}
	IPR[2]=320;
	IPR[3]=0;
	goto MMSD;
MMSC_400:
	if ((IWRD[0][10] > 1) && (IWRD[1][8] != 3) && (IWRD[1][9] != 93)) {
		IPR[2]=320;
		IPR[3]=0;
		goto MMSD;
	}
	IWRD[0][10]=2;
	IWRD[0][9]=1;
	IWRD[0][8]=2;
	if ((IR == 93) || (IR == 91)) IWRD[1][9]=1;
	if (IR == 95) goto MMSC_1000;
	IPR[2]=410;
	IPR[3]=0;
	goto MMSD;
MMSC_500:
	if (IR == 17) goto MMSC_800;
	if ((IR < 55) || (IR > 83)) {
		IPR[2]=520;
		IPR[3]=0;
		goto MMSD;
	}
	ITST[1]=54;
	IR=54;
	printf("\nYOU ARE ENVELOPED IN A CLOUD OF SMOKE THAT QUICKLY "
		"DISAPPEARS\n");
	goto MMSC_1141;
MMSC_600:
	if (IR == 98) {
		IPR[2]=520;
		IPR[3]=0;
		goto MMSD;
	}
	ITST[1]=98;
	IR=98;
	printf("\nYOU ARE ENVELOPED IN A CLOUD OF SMOKE THAT QUICKLY "
		"DISAPPEARS\n");
	goto MMSC_1141;
MMSC_700:
	if (IR == 17) goto MMSC_800;
	if ((IR < 54) || (IR > 83)) {
		IPR[2]=520;
		IPR[3]=0;
		goto MMSD;
	}
	if (IWRD[0][9] == 6) IR=81;
	if (IWRD[0][9] == 10) IR=82;
	ITST[1]=IR;
	printf("\nYOU ARE ENVELOPED IN A CLOUD OF SMOKE THAT QUICKLY "
		"DISAPPEARS\n");
	goto MMSC_1141;
MMSC_800:
	if (((IWRD[0][9] == 5) && (IVEN[88] != 17)) || 
	    ((IWRD[0][9] == 6) && (IVEN[88] != 117)) ||
	    ((IWRD[0][9] == 9) && (IVEN[88] != 217)) ||
	    ((IWRD[0][9] == 10) && (IVEN[88] != 317))) {
		IPR[2]=520;
		IPR[3]=0;
		goto MMSD;
	}
	if (IVEN[88] == 1017) {
		IPR[2]=520;
		IPR[3]=0;
		goto MMSD;
	}
	IVEN[88] += 100;
	if (IVEN[88] == 417) {
		IPR[2]=810;
		IPR[3]=0;
		goto MMSD;
	}
	IPR[2]=1040;
	IPR[3]=0;
	goto MMSD;
MMSC_900:
	if (IWRD[0][10] != 1) {
		IPR[2]=320;
		IPR[3]=0;
		goto MMSD;
	}
	if ((IR < 28) || ((IR > 80) && (IR < 97))) {
		IPR[2]=910;
		IPR[3]=0;
		goto MMSD;
	}
	if (IVEN[32] < 20000) {
		IPR[2]=920;
		IPR[3]=0;
		goto MMSD;
	}
	R = RN(R);
	if ((IR < 62) || ((IR > 80) && (IR < 97)) || 
	    ((IR > 55) && (IR < 78)) || (R < 0.667)) {
		IPR[2]=1040;
		IPR[3]=0;
		goto MMSD;
	}
	/*
	 * Hmm.  Note that some mapping changes here will never happen.
	 * This bug comes straight from the original... should we fix it?
	 * Leaving it here for now.  Update 2/29/200: I've decided to fix
	 * it.  Original is in comments, fixes follow.
	 */
	if (IR == 99) IR = 28;
	if (IR == 98) IR = 33;
	if (IR == 97) IR = 34;
/*
	if (IR == 62) IR = 43;
	if (IR == 62) IXT[341]=4362;
	if (IR == 63) IR=45;
	if (IR == 63) IXT[342]=4563;
	if (IR == 64) IR=48;
	if (IR == 64) IXT[343]=4864;
	if (IR == 65) IR=50;
	if (IR == 65) IXT[344]=5065;
	if (IR == 78) IR=29;
	if (IR == 78) IXT[345]=2978;
	if (IR == 79) IR=33;
	if (IR == 79) IXT[346]=3379;
	if (IR == 80) IR=29;
	if (IR == 80) IXT[347]=2980;
*/
	if (IR == 62) IXT[341]=4362;
	if (IR == 62) IR = 43;
	if (IR == 63) IXT[342]=4563;
	if (IR == 63) IR=45;
	if (IR == 64) IXT[343]=4864;
	if (IR == 64) IR=48;
	if (IR == 65) IXT[344]=5065;
	if (IR == 65) IR=50;
	if (IR == 78) IXT[345]=2978;
	if (IR == 78) IR=29;
	if (IR == 79) IXT[346]=3379;
	if (IR == 79) IR=33;
	if (IR == 80) IXT[347]=2980;
	if (IR == 80) IR=29;

	ITST[1]=IR;
	ITST[14]=IR;
	printf("\nYOU DUG AND FELL INTO AN UNDERGROUND TUNNEL\n");
	goto MMSC_1141;
MMSC_950:
	ITST[22] += 1;
	if (ITST[38] != 0) {
		IPR[2]=0;
		IPR[3]=0;
		goto MMSB;
	}
	if (ITST[22] == 1) {
		IPR[2]=960;
		IPR[3]=0;
		goto MMSD;
	}
	IPR[2]=970;
	IPR[3]=0;
	goto MMSD;
MMSC_970:
	if ((IWRD[1][8] != 4) || (IWRD[0][10] != 10)) {
		IPR[2]=320;
		IPR[3]=0;
		goto MMSD;
	}
	if (IFSD(IRES[IWRD[1][9]]) != IR) {
		IPR[2]=980;
		IPR[3]=0;
		goto MMSD;
	}
	goto MMSC_1105;
MMSC_1000:
	if (IWRD[0][10] > 2) {
		IPR[2]=320;
		IPR[3]=0;
		goto MMSD;
	}
	IC = IWRD[1][9];
	if (IWRD[0][10] == 1) IC = 9;
	IRX=0;
	if ((IWRD[1][9] == 110) && ((IR == 97) || (IR == 98))) IRX = 84;
	if (IRX==84) {
		IR=84;
		ITST[1]=84;
		goto MMSC_1025;
	}
	if ((IWRD[0][10]==2) && (IWRD[1][8] != 2)) {
		IPR[2]=1050;
		IPR[3]=0;
		goto MMSD;
	}
	if ((IC == 11) &&  (((ITST[5]-1) != (ITST[4]/1000)*1000) ||
	    (IR == 97) || (IR == 98))) {
		IPR[2]=1002;
		IPR[3]=0;
		goto MMSD;
	}
	if (IC==11) {
		ITST[1]=ITST[14];
		ITST[14]=IR;
		IR=ITST[1];
		goto MMSC_1025;
	}
	if ((IC < 5) && (((IR < 54) && (IR != 35)) || (IR == 98)) &&
	    (IVEN[6] < 20000)) {
		IPR[2]=1005;
		IPR[3]=0;
		goto MMSD;
	}
	if (IC < 7) goto MMSC_1007;
	IC = IC-9+ITST[4]/1000;
	if (IC > 4) IC -= 4;
	if (IC < 1) IC += 4;
MMSC_1007:
	IFC = ITST[4]/1000;
	if (IC < 5) ITST[4] = ITST[4] - (ITST[4]/1000)*1000 + IC*1000;
	for (J=IC*60 - 59; J<=(IC*60); J++) {
		if (IFSD(IXT[J]) == IR) goto MMSC_1015;
	}
	IPR[2]=1105;
	IPR[3]=0;
	goto MMSD;
MMSC_1015:
	if ((J != 93) && (J != 212) && (J != 281) && (J != 351)) goto MMSC_1020;
	if (IR < 88) goto MMSC_1020;
	for (K=1; K<=53; K++) 
		if ((IVEN[K] > 20000) && ((J == 281) || (ITD(IVEN[K] > 1)))) {
			IPR[2]=1017;
			IPR[3]=0;
			goto MMSD;
		}
	
MMSC_1020:
	if (((IR < 55) || (IR == 90)) && (IXT[J] > 20000)) {
		IPR[2]=1021;
		IPR[3]=0;
		goto MMSD;
	}
	IX=0;
	if ((IR == 91) && ((ITST[5]-1) != ITST[13])) IX=99;
	if (IX == 99) {
		ITST[1]=2;
		IR=2;
		printf("\nYOU JUST FELL THROUGH THE LOOSE BOARDS.\n");
		goto MMSC_1025;
	}
	R=RN(R);
	if ((R > 0.5) || (IR != 39)) goto MMSC_1024;
	if (R > 0.25) IR=41;
	ITST[1]=IR;
	printf("\nYOU SLIPPED ON THE LADDER.\n");
	goto MMSC_1025;
MMSC_1024:
	if ((IR > 54) && (IXT[J] > 20000)) {
		IPR[2]=1023;
		IPR[3]=0;
		goto MMSD;
	}
	IRB=ITST[1];
	ITST[14]=IRB;
	ITST[1]=ITFD(IXT[J]);
	IR=ITST[1];
MMSC_1025:
	ITST[4]=(ITST[4]/1000)*1000+ITST[5];
	if (IR == 0) {
		IPR[2]=1026;
		IPR[3]=0;
		goto MMSD;
	}
	if ((IR == 46) && (IXT[320] == 0)) {
		IPR[2]=1027;
		IPR[3]=0;
		goto MMSD;
	}
	if ((IVEN[22]==0) && ((ITST[5]-(ITST[5]/50)*50) < 10) && (IR != 35))
		IVEN[22] = 100+IR;
	if ((IVEN[22] != 0) && (IVEN[22] < 20000) && 
	    ((ITST[5]-(ITST[5]/50)*50) > 40) && (IFSD(IVEN[22]) != IR))
		IVEN[22]=0;
	if ((((IR != 97) && (IR != 98)) || (IWRD[0][9] == 25))) goto MMSC_1045;
	for (I=1; I<=2; I++) {
		R=RN(R);
		for (J=1; J<=(int)(R*10+10); J++) {
			R1=RN(R);
			IC=IXT[60*(int)(R1*4)+58+I];
			if ((IR==98) && (ITST[17] > 1) && (IC==9798))
				IC=3598;
			if ((IR==98) && (ITST[17] < 2) && (IC==3598))
				IC=9798;
			R=RN(R1);
			IXT[60*(int)(R1*4)+58+I]=IXT[60*(int)(R*4)+58+I];
			IXT[60*(int)(R*4)+58+I]=IC;
		}
	}
	for (J=1; J<=35; J++) {
		if ((IVEN[J] >= 10000) || ((IFSD(IVEN[J]) != IR) &&
		    (IFSD(IVEN[J]) != 0)) || IVEN[J] == 0) continue;
		if (IFSD(IVEN[J]) == IR) IVEN[J] = IVEN[J] - IR;
		if (IFSD(IVEN[J]) > 0) continue;
		R=RN(R);
		if (R > 0.1) continue;
		IVEN[J] += IR;
	}
	if ((IXT[59] == 9497) && (IXT[179] == 9897)) IXT[59]=9597;
	if ((IXT[119] == 9297) || (IXT[119] == 9597)) IXT[119]=9497;
	if ((IXT[179] == 9497) && (IXT[59] == 9997)) IXT[179]=9297;
	if ((IXT[239] == 9297) || (IXT[239] == 9597)) IXT[239]=9497;
	if ((IRES[6] == 893) && (IRES[12] == 993)) IXT[54]=29293;
	if ((IRES[6] == 892) && (IRES[12] == 992)) IXT[54]=29293;
	if (IXT[54] == 29293) IXT[234]=29392;
	R=RN(R);
	if (R > 0.2) goto MMSC_1045;
	R=RN(R);
	for (J=(int)(R*35)+1; J<=35; J++) {
		if (J == 22) continue;
		if ((IVEN[J] > 10000) || (IFSD(IVEN[J]) != 0) ||
		    (IVEN[J] == 0)) continue;
		printf("\nA WOOD NYMPH RAN BY CARRYING ");
		MMRI(4, J);
		goto MMSC_1045;
	}
MMSC_1045:
	IC=1;
	if ((IR < 42) | (IR > 53) || (IRES[4] >= 10000) ||
	    (IFSD(IRES[4]) == 52)) goto MMSC_1080;
	IMZ[1]=0;
	IMZ[2]=0;
	IMZ[3]=0;
	IC=0;
	RND=RND*7.7-floor(RND*7.7);
	IX=(int)(3*RND+1);
	for (I=1; I<=360; I++) {
		if ((IFSD(IXT[I]) != IFSD(IRES[4])) ||
		    (ITFD(IXT[I]) < 42) || (ITFD(IXT[I]) > 51)) continue;
		IMZ[++IC]=I;
		if (IC == IX) goto MMSC_1055;
	}
	goto MMSC_1080;
 MMSC_1055:
	RND=RND*7.7-(int)(RND*7.7);
	IX=(int)(360*RND+1);
	
	for (J=IX; J<=360; J++) {
		if ((IXT[J] < 4100) || (IFSD(IXT[J]) != 0)) continue;
		if ( (((IMZ[1]-1)/60) == ((J-1)/60)) || 
		     (((IMZ[2]-1)/60) == ((J-1)/60)) || 
		     (((IMZ[3]-1)/60) == ((J-1)/60)) ) continue;
		IXX=IXT[J];
		IXT[J]=IXT[I];
		IXT[I]=IXX;
		goto MMSC_1075;
	}
	for (J=1; J<=IX; J++) {
		if ((IXT[J] < 4100) || (IFSD(J) != 0)) continue;
		for (K=1; K<=3; K++) {
			if ( (((IMZ[1]-1)/60) == ((J-1)/60)) || 
			     (((IMZ[2]-1)/60) == ((J-1)/60)) || 
			     (((IMZ[3]-1)/60) == ((J-1)/60)) ) continue;
			IXX=IXT[J];
			IXT[J]=IXT[I];
			IXT[I]=IXX;
			goto MMSC_1075;
		}
	}
	goto MMSC_1080;
MMSC_1075:
	RND=RND*7.7-(int)(RND*7.7);
	if ((IC < 2) || (RND > 0.3)) goto MMSC_1080;
	for (I=1; I<=360; I++) {
		if ( ((IFSD(IXT[I])) == (ITFD(IXT[J]))) &&
		     ((ITFD(IXT[I])) == (IFSD(IXT[J]))))
			IXT[I]=100*(IXT[I]/100);
	}
	IXT[J]=100*(IXT[J]/100);
MMSC_1080:
	if ((IR > 35) && (IR < 41)) ITST[12]=ITST[12]-ITST[5]+ITST[13];
	if ((IR > 35) && (IR < 41) && (ITST[12] > 0)) 
		printf("\nTHE CORRIDOR WALLS ARE CLOSING IN\n");
	if ((IR > 35) && (IR < 40) && (IVEN[17] > 20000) &&
	    (ITST[12] <= 0)) {
		IPR[2]=1084;
		IPR[3]=0;
		goto MMSD;
	}
	if ((IR == 40) && (ITST[12] <= 0)) {
		IPR[2]=1086;
		IPR[3]=0;
		goto MMSD;
	}
	if ((IR > 35) && (IR < 40) && (ITST[12] <= 0)) {
		IPR[2]=1088;
		IPR[3]=0;
		goto MMSD;
	}
	if ((IR == 4) || (IR == 41))
		printf("\nTHE CORRIDOR WALLS HAVE OPENED\n");
	if ((IR == 4) || (IR == 41)) ITST[12]=4;
	if (IR == 4) IXT[140]=3640;
	if (IR == 4) IXT[314]=3940;
	if ((ITST[5] <= 300) || (IR < 55) || (IR > 82)) goto MMSC_1097;
	R=RN(R);
	if ((R > 0.5) && (R < 0.8)) 
		printf("\nYOU JUST NOTICED A SNAKE SLITHER BY.\n");
	if ((R > 0.8) && (R < 0.95)) 
		printf("\nYOU WERE JUST BITTEN BY A SNAKE THAT RACED BY.\n");
	if (R > 0.95) {
		IPR[2]=1093;
		IPR[3]=0;
		goto MMSD;
	}
MMSC_1097:
	if (((IR == 71) || (IR == 72)) && (IRES[15] < 10000))
		printf("\nA WOLF IS RUNNING TOWARDS YOU\n");
	if (((IR == 71) || (IR == 72)) && (IVEN[24] < 20000) &&
	     (IRES[15] < 10000)) {
		IPR[2]=1097;
		IPR[3]=0;
		goto MMSD;
	}
	if (((IR == 71) || (IR == 72)) && (IRES[15] < 10000)) {
		IPR[2]=1096;
		IPR[3]=0;
		goto MMSD;
	}
	if (IR != 1) IXT[1]=0;
	if (IR != 5) IXT[183]=0;
	if (IR != 7) IXT[123]=0;
	if (IR != 10) IXT[5]=0;
	if (IR != 14) IXT[187]=0;
	if (IR != 16) IXT[128]=0;
	if (IR != 19) IXT[11]=0;
	if (IR != 23) IXT[191]=0;
	if (IR != 25) IXT[134]=0;
	if (IR == 93) ITST[17]=0;
	if ((IR == 35) && (IWRD[1][9] == 3)) ITST[17] += 1;
	if ((IR == 35) && (IWRD[1][9] == 1)) ITST[17] -= 1;
	if ((IR == 35) && (((IFC+IWRD[1][9]) == 10) ||
	    ((IFC+IWRD[1][9])==14))) ITST[17] -= 1;
	if ((IR == 35) && (((IFC+IWRD[1][9]) == 8) ||
	    ((IFC+IWRD[1][9])==12))) ITST[17] += 1;
	if (ITST[17] == 1) IXT[47]=9335;
	if (ITST[17] == 2) IXT[47]=3535;
	IC = 100-ITST[17]/10;
	ID = ITST[17]/10;
	if ((IR == 35) && ((ITST[17]%10) == 0) && (ITST[5] < 300))
		printf("\nYOU PASSED A MILEAGE SIGN THAT READ: BIG CITY %d   "
			"MYSTERY MANSION %d\n", IC, ID);
	if ((IFD(IVEN[35]) == 2) && (ITD(IVEN[35]) == 6)) {
		IVEN[35] -= 100;
		IPR[2]=10210;
		IPR[3]=0;
		goto MMSG;
	}
	for (J=2; J<=16; J++) {
		if (J == 15) continue;
		if ((IFSD(IRES[J]) == 0) || (IRES[J] > 10000)) continue;
		if (IFSD(IRES[J]) == IRB) goto MMSC_1120;
MMSC_1105:
		R=RN(R);
		for (K=(int)(R*6)*60+1; K<=(int)(R*6)*60+60; K++) {
			if (IFSD(IXT[K]) != IFSD(IRES[J])) continue;
			if (IXT[K] > 20000) break;
			IRC=IFSD(IRES[J]);
			IRES[J]=(IRES[J]/100)*100+ITFD(IXT[K]);
			goto MMSC_1130;
		}
		continue;
MMSC_1120:
		IRES[J]=IRES[J]-IRB+IR;
		IRC=IRB;
MMSC_1130:
		/* Set IC == to NPC #J's current location */
		IC=IRES[J]-(IRES[J]/100)*100;
		if (((J == 3) || (J == 14)) && (((IC > 26) &&
		   (((ITST[5] < 400) || (IFSD(J) != IR)))) || (IC == 0)))
			IRES[J]=100*(IRES[J]/100)+IRC;
		if ((J == 4) && ((IC > 51) || (IC < 43))) 
			IRES[4]=100*(IRES[4]/100)+IRC;
		/*
		 * Vampire.  If he was moved into the charming chapel, or 
		 * if the room he was moved into was lit and it's before
		 * nightfall, or if he was put into the haunted hallway,
		 * or anywhere outside, then...
		 */
		if ((J == 5) && ((IC == 16) || 
                                 ((IROM[IC] >= 20000) && (ITST[5] < 300)) || 
                                 (IC > 27) || 
                                 (IC  == 21)))
			IRES[5]=100*(IRES[5]/100)+IRC;
		if (((J ==6) || (J == 12)) && ((IC < 92) || (IC == 98)))
			IRES[J]=100*(IRES[J]/100)+IRC;
		if (((J == 8) || (J == 2)) && ((IC < 9) || (IC > 77)))
			IRES[J]=100*(IRES[J]/100)+IRC;
		if (((J == 9) || (J == 7)) && 
		   (((IC > 18) && (IC < 54)) || (IC > 77)))
			IRES[J]=100*(IRES[J]/100)+IRC;
		if ((J == 10) && ((IC == 98) || (IC == 78) || (IC == 80) ||
		    (IC == 88) || (IC == 83) || (IC == 0)))
			IRES[10]=100*(IRES[10]/100)+IRC;
		if ((J == 13) && ((IC < 29) || (IC > 31))) 
			IRES[13]=100*(IRES[13]/100)+IRC;		
		if ((J == 11) && ((IC < 55) || (IC > 77))) 
			IRES[11]=100*(IRES[11]/100)+IRC;		
		if ((J == 16) && ((IC < 93) || (IC > 96))) 
			IRES[16]=100*(IRES[16]/100)+IRC;		
	}
MMSC_1141:
	ITST[13]=ITST[5];
	if (IWRD[0][9] == 13) IR=IFSD(IRES[IWRD[1][9]]);
	ITST[1]=IR;
	if ((IR == ITST[8]) && (IFSD(IRES[ITST[9]]) == IR) &&
	    ( (IVEN[ITST[10]] > 20000) || 
              ((IFSD(IVEN[ITST[10]]) == IR) &&
	       (IVEN[ITST[10]] < 10000)))) {
		IPR[2]=1145;
		IPR[3]=0;
		goto MMSD;
	}
MMSC_1142:
	for (I=1; I<=3; I++) {
		if (I == 1) K=4;
		if (I == 2) K=19;
		if (I == 3) K=27;
		if ((IFD(IVEN[K]) == 1) && 
		    (((IVEN[K] < 10000) && (IFSD(IVEN[K]) == IR)) || 
		     (IVEN[K] > 20000) || 
		     ((IVEN[K] > 10000) && (IVEN[K] < 20000) &&
		      (IFSD(IRES[IVEN[K]]) == IR)))) goto MMSC_1190;
	}
	if ((IR == 10) && (IVEN[45] > 1000)) goto MMSC_1190;
	if ((IR == 14) && (IVEN[89] > 1000)) goto MMSC_1190;
	if (IVEN[31] > 21000) goto MMSC_1190;
	if ((IR > 53) && (ITST[5] < 300)) goto MMSC_1190;
	if (IR > 53) {
		IPR[2]=1150;
		IPR[3]=1;
		goto MMSD;
	}
	if ((IROM[IR] > 20000) && (ITST[5] < 300)) goto MMSC_1190;
	if (IROM[IR] > 20000) {
		IPR[2]=1150;
		IPR[3]=1;
		goto MMSD;
	}
	if ((ITST[5] < 300) && (IROM[IR] > 10000)) {
		IPR[2]=1150;
		IPR[3]=2;
		goto MMSD;
	}
	IPR[2]=1150;
	IPR[3]=0;
	goto MMSD;
MMSC_1190:
	ISC=ITFD(IROM[IR]);
	ITST[2] = ITST[2]+ISC-50;
	IROM[IR]=IROM[IR]-(ISC*100)+5000;
	if (ISC < 50) ITST[3]=ITST[3]-50+ISC;
	IC=1;
	if ((IR == 35) || (IR == 54) || (IR == 71) || (IR == 72) ||
	    (IR == 81) || (IR == 82) || (IR == 85) || (IR == 94) ||
	    (IR == 96) || (IR == 86) || (IR == 92) || (IR == 91))
		IC=2;
	if (((IR >= 73) && (IR <= 77)) || (IR == 99)) IC=3;
	if (((IR >= 55) && (IR <= 57)) || (IR == 93) || (IR == 95)) IC=4;
	printf("\nYOU ARE %c%c THE ", IPRP[IC][0], IPRP[IC][1]);
	for (L=0; L<16; L++) 
		putchar(IRNM[IFSD(IROM[IR])][L]);
	if ((IVEN[19] > 21000) && ((IR == 21) || (IR == 36) ||
	    (IR == 54) || (IR == 92))) {
		IPR[2]=213;
		IPR[3]=0;
		goto MMSD;
	}
	if ((ITST[8] == IR) && (ITST[10] > 8) && (ITST[10] < 13))
		printf("\nTHERE IS BLOOD HERE");
	MMRI(1, 0);
	IC=0;
	for (J=1; J<=17; J++) {
		if (IR != IFSD(IRES[J])) continue;
		if (IC == 0) {
			printf("\n\nYOU ARE HERE WITH: ");
			for (L=0; L<8; L++)
				putchar(IRSN[J][L]);
		}
		if (IC == 1) {
			printf("\n                   ");
			for (L=0; L<8; L++)
				putchar(IRSN[J][L]);
		}
		IC=1;
	}
	if ((ITST[5] < 450) && ((IR < 28) || (IR > 64) || ((IR > 34) &&
	    (IR < 42)) || ((IR > 52) && (IR < 58)))) goto MMSC_1620;
	J2=(int)(R*240+1);
MMSC_1585:
	for (J=J2; J<=360; J++) {
		if (IFSD(IXT[J]) != IR) continue;
		J1=(J-1)/60+1;
		if (((IR > 53) && (IR != 98)) || (IVEN[6] > 20000) ||
		    (IR == 35) || (J1 > 4)) goto MMSC_1590;
		J1=9+J1-ITST[4]/1000;
		if (J1 > 10) J1=J1-4;
		if (J1 < 7) J1=J1+4;
MMSC_1590:
		printf("\n\nTHERE IS A WAY TO GO ");
		for (L=0; L<8; L++) 
			putchar(IDTN[J1][L]);
		goto MMSC_1620;
	}
	if (J == 1) goto MMSC_1620;
	J2=1;
	goto MMSC_1585;
MMSC_1620:
	if ((ISC != 50) || (IWRD[0][9] == 25)) {
		IPR[2]=IR;
		IPR[3]=0;
		goto MMSE;
	}
	IPR[2]=0;
	IPR[3]=0;
	goto MMSB;

MMSD:
	IRIT = IPR[2];
	if (IRIT == 11111) goto MMSD_99999;
	if ((ITST[9] != 2) && (IRES[2] >= 10000)) ITST[24]=1;
	if ((ITST[9] != 6) && (IRES[6] >= 10000)) ITST[24]=1;
	if ((ITST[9] != 7) && (IRES[7] >= 10000)) ITST[24]=1;
	if ((ITST[9] != 8) && (IRES[8] >= 10000)) ITST[24]=1;
	if ((ITST[9] != 9) && (IRES[8] >= 10000)) ITST[24]=1;
	if ((ITST[9] != 12) && (IRES[12] >= 10000)) ITST[24]=1;
	if ((ITST[9] != 14) && (IRES[14] >= 10000)) ITST[24]=1;
	if (IRES[5] == 0) IRES[5] = 100;
	if ((ITD(IRES[5]) != 1) && (ITST[5] > 300)) ITST[3]=ITST[3]-75;
	if ((ITD(IRES[5]) != 1) && (ITST[5] > 300)) IRES[5] += 100;
	if (IRIT == 6005) goto MMSD_80000;

	if (IRIT == 110) 
		printf("\nYOU CLUMSY FOOL! YOU TRIPPED "
			"AND DROPPED EVERYTHING.");
	if (IRIT == 150)
		printf("\nA WORD IS TOO LONG");
	if (IRIT == 170)
		printf("\nTHERE ARE TOO MANY WORDS");
	if (IRIT == 160) {
		printf("\nMY INPUT VOCABULARY DOESN'T INCLUDE THE WORD ");
		for (L=0; L<8; L++)
			putchar(IWRD[IPR[3]][L]);
	}
	if (IRIT == 165) 
		printf("\nWATCH YOUR LANGUAGE!");
	if (IRIT == 166) 
		printf("\nI WILL NOT ANSWER ANY QUESTIONS. I ONLY RESPOND "
			"TO YOUR ACTIONS.");
	if (IRIT == 176) {
		printf("\nYOU COULD NOT HOLD YOUR BREATH ANY LONGER AND "
			"DROWNED.\nYOU SHOULD HAVE GONE BACK WHILE YOU COULD.");
		goto MMSD_80000;
	}
	if (IRIT == 180) 
		printf("\nI EXPECT TO SEE A VERB AS THE FIRST WORD");

	if (IRIT != 185) goto MMSD_187;
	printf("\nYOU HEAR AN EXPLOSION AND A LOW RUMBLING AS IF SOME OF "
		"THE PASSAGES UNDER\nTHE MANSION AND GROUNDS HAVE CAVED-IN. "
		"YOU ARE BADLY SHAKEN AND THERE IS\nDESTRUCTION ALL AROUND "
		"YOU. YOU CAN SMELL SMOKE.");
	for (J=301; J<=360; J++) 
		IXT[J]=0;
	IXT[173]=0;
	IXT[26]=0;
	IXT[198]=0;
	IXT[137]=0;
	IXT[46]=0;
	IXT[237]=0;
	IXT[197]=0;
MMSD_187:
	if (IRIT == 189) {
		printf("\nYOU HAVE SUFFOCATED FROM THE SMOKE AND YOUR BODY IS "
			"SLOWLY BEING\nCONSUMED BY FLAMES AS ALL OF MYSTERY "
			"MANSION GOES UP IN SMOKE.");
		goto MMSD_90020;
	}
	if (IRIT == 1017) 
		printf("\nYOU CANNOT QUITE GET THROUGH THE SMALL DOOR.");
	if (IRIT == 1027) 
		printf("\nYOU ENTERED A TUNNEL FILLED OVER YOUR HEAD WITH THE "
			"WATER RUSHING\nIN FROM THE PRESSURE OF THE INCOMING "
			"TIDE.");
	if (IRIT == 1093) {
		printf("\nYOU JUST STUMBLED INTO THE MIDDLE OF A BUNCH OF "
			"POISONOUS SNAKES AND\nMANY OF THEM BIT YOU AND "
			"ARE CRAWLING ALL OVER YOUR DEAD BODY.");
		goto MMSD_80000;
	}
	
	if (IRIT != 191) goto MMSD_1199;
	printf("\nYOU HAVE BEATEN THE ODDS AND HAVE DONE THE IMPOSSIBLE. "
		"YOU HAVE SURVIVED\nMYSTERY MANSION AND YOU CAN NOW SEE IT "
		"GOING UP IN SMOKE BEFORE YOU.");
	IC=0;
	for (I=1; I<=52; I++)
		if (IVEN[I] > 20000) IC += IVAL[I];
	
	printf("\nYOUR SCORE INCLUDES %d POINTS FOR THE ITEMS YOU HAVE "
		"WITH YOU.", IC);
	ITST[2] += IC;
	if (IXT[52] == 0) {
		ITST[2] += 10;
		printf("\nYOUR SCORE ALSO INCLUDES 10 POINTS BECAUSE THE TAXI "
			"IS WAITING.");
	}
	if (ITST[2] > 999)
		ITST[2] = 999;
	if (IXT[52] != 0)
		printf("\nNOW YOU HAVE TO WALK TO THE BIG CITY.");
	goto MMSD_90020;

MMSD_1199:
	if (IRIT == 190) 
		printf("\nI CANNOT DECIPHER TWO VERB TYPE WORDS IN ONE "
			"SENTENCE");
	if (IRIT == 192) {
		printf("\nI GIVE UP ON YOU. YOU HAVE GOTTEN YOURSELF INTO A "
			"SITUATION I CANNOT GET\nYOU OUT OF.");
		goto MMSD_90020;
	}
	if (IRIT == 213) {
		printf("\nA SLIGHT BREEZE BLEW OUT YOUR CANDLE.");
		IVEN[19]=20200;
	}
	if (IRIT == 244) {
		printf("\nTHE BOMB HAS EXPLODED AND BLOWN YOU TO BITS");
		goto MMSD_80000;
	}
	if (IRIT == 251) {
		printf("\nTHE MURDERER WAS AFRAID YOU WOULD CALL THE POLICE "
			"AND HAS KILLED YOU.");
		goto MMSD_80000;
	}
	if (IRIT == 254) {
		printf("\nTHE DWARF JUST THREW A HATCHET AT YOU AND "
			"SPLIT YOUR HEAD OPEN");
		goto MMSD_80000;
	}
	if (IRIT == 262) {
		printf("\nTHE VAMPIRE HAS ATTACKED AND SUCKED THE BLOOD OUT "
			"OF YOU");
		goto MMSD_80000;
	}
	if (IRIT == 285) {
		printf("\nTHE WEREWOLF HAS ATTACKED YOU AND TORN YOU TO BITS.");
		goto MMSD_80000;
	}
	if (IRIT == 287) 
		printf("\nTHERE ARE %d SCREAMING DEMONS IN THE WAY.", IPR[3]);
	if (IRIT == 290) 
		printf("\nABSOLUTELY NOTHING HAPPENS!");
	if (IRIT == 320) 
		printf("\nI CANNOT FIGURE OUT WHAT YOU ARE TRYING TO SAY");
	if (IRIT == 340) 
		printf("\nTHE WALL IS TOO HIGH");
	if (IRIT == 350) 
		printf("\nI DON'T SEE A WALL AROUND HERE");
	if (IRIT == 360) 
		printf("\nYOU HAVE TO BE IN THE WOODS TO CLIMB TREES");
	if (IRIT == 370) 
		printf("\nYOU WOULD PROBABLY JUST FALL OUT OF THE TREE. I "
			"LOSE MORE PLAYERS\nLIKE THAT. SO I CANNOT LET YOU "
			"CLIMB ANY TREES.");
	if (IRIT == 380) 
		printf("\nTHE GARDENER HAS CAUGHT YOU AND FORCEFULLY, HAS "
			"TAKEN YOU\nTO THE VERANDA AND HAS TOLD YOU TO "
			"LEAVE IF YOU ARE GOING TO ACT\nLIKE THAT");
	if (IRIT == 385) 
		printf("\nTHE GATE IS COVERED WITH ALL KINDS OF POINTS AND "
			"BARBS AND YOU\nWOULD NEVER MAKE IT OVER ALIVE.");
	if (IRIT == 390) 
		printf("\nYOU ARE NOT BY THE GATE.");
	if (IRIT == 410) 
		printf("\nI DIDN'T EXPECT YOU WOULD USE ENTER HERE.");
	if (IRIT == 510) 
		printf("\nYOU ARE SUDDENLY SURROUNDED BY SMOKE AND WHEN IT "
			"CLEARS YOU SEE\nTHAT YOU ARE ON THE VERANDA");
	if (IRIT == 520) 
		printf("\nNOTHING HAPPENS");
	if (IRIT == 610) 
		printf("\nYOU ARE SUDDENLY SURROUNDED BY SMOKE AND WHEN IT "
			"CLEARS YOU SEE\nTHAT YOU ARE IN THE WOODS");
	if (IRIT == 710) 
		printf("\nYOU ARE SUDDENLY SURROUNDED BY SMOKE AND WHEN IT "
			"CLEARS YOU SEE\nTHAT YOU ARE ON A BRIDGE");
	if (IRIT == 810) {
		printf("\nTHE SCROLL MAGICALLY UNROLLS ON THE TABLE AND YOU "
			"READ THAT THE ");
		for (L=0; L<8; L++)
			putchar(IRSN[ITST[9]][L]);
		printf("\nIS THE MURDERER AND THAT THE SCENE OF CRIME IS THE ");
		for (L=0; L<16; L++)
			putchar(IRNM[IFSD(IROM[ITST[8]])][L]);
		printf("\nAND THAT THE MURDER WEAPON IS THE %s", IWP);
	
	}
	if (IRIT == 910) 
		printf("\nYOU CANNOT DIG HERE.");
	if (IRIT == 920)
		printf("\nYOU WON'T GET VERY FAR WITHOUT A SHOVEL.");
	if (IRIT == 960)
		printf("\nLET THAT HAPPEN AGAIN AND THE GAME WILL ABORT");
	if (IRIT == 970) {
		printf("\nGAME ABORTED DUE TO NO INPUT");
		goto MMSD_90020;
	}
	if (IRIT == 980) {
		printf("\nFIRST YOU HAVE TO FIND THE ");
		for (L=0; L<8; L++) 
			putchar(IRSN[(int)(IWRD[1][9])][L]);
	}
	if (IRIT == 1002) 
		printf("\nI FORGOT WHERE YOU WERE");
	if (IRIT == 1005) 
		printf("\nYOU NEED A COMPASS TO TELL CARDINAL POINTS HERE");
	if (IRIT == 1021) 
		printf("\nTHE DOOR IS LOCKED");
	if (IRIT == 1023) 
		printf("\nTHE GATE IS CLOSED");
	if (IRIT == 1026) {
		printf("\nYOU JUST FELL DOWN THE 500 FOOT CLIFF ONTO THE "
			"ROCKS BELOW.");
		goto MMSD_80000;
	}
	if (IRIT == 1032)
		goto MMSD_90060;
	if (IRIT == 1040)
		printf("\nOKAY.");
	if (IRIT == 1050) {
		printf("\nYOU CANNOT GO "); 
		for (L=0; L<8; L++) 
			putchar(IWRD[1][L]);
	}
	if (IRIT == 1065) 
		printf("\nPROGRAM ERROR AT LINE %d", IPR[3]);
	if (IRIT == 1084) {
		printf("\nTHE AMULET SAVED YOU FROM BEING CRUSHED "
			"BY THE WALLS");
		ITST[1]=4;
	}
	if (IRIT == 1086) {
		printf("\nTHE CLOSING CORRIDOR WALLS HAVE BLOCKED THE EXITS "
			"AND HAVE TRAPPED YOU HERE\n");
		IXT[314]=0;
		IXT[140]=0;
	}
	if (IRIT == 1088) {
		printf("\nTHE CLOSING WALLS HAVE CRUSHED YOU\n");
		goto MMSD_80000;
	}
	if (IRIT == 1096) {
		printf("\nTHE WOLF ATTACKED YOU AND IS BUSY EATING YOUR FOOD");	
		IVEN[24]=0;
		IRES[15]=ITST[1];
	}
	if (IRIT == 1097) {
		printf("\nTHE WOLF ATTACKED YOU RAVENOUSLY AND KILLED YOU");
		goto MMSD_80000;
	}
	if (IRIT == 1105) 
		printf("\nYOU CANNOT GO THAT WAY.");
	if (IRIT == 1145) {
		printf("\nCONGRATULATIONS! YOU SOLVED THE MYSTERY BY HAVING "
			"THE %s", IWP);
		printf("\nWHICH IS THE MURDER WEAPON, AT THE SCENE OF THE "
			"CRIME WITH THE ");
		for (L=0; L<8; L++) 
			putchar(IRSN[ITST[9]][L]);
		printf("\nWHO IS THE MURDERER");
		ITST[2] += 200;
		ITST[8]  = 0;
	}
	if (IRIT == 1150) {
		printf("\nIT IS DARK HERE\n");
		if (IPR[3] == 1)
			printf("\nIT IS AFTER SUNSET");
		if (IPR[3] == 2)
			printf("\nA CURTAIN BLOCKS THE LIGHT");
	}
	if (IRIT == 10030)
		goto MMSD_80000;
	if (IRIT == 13010) {
		printf("\nTHE POISON DID ITS JOB AND YOU DIED IN A "
			"FIT OF AGONY.");
		goto MMSD_80000;
	}
/*
	goto MMSD_999;
*/

	IRIT=0;
	IPR[2]=IPR[3]=0;
	goto MMSB;
	
MMSD_80000:
	IC=0;
	if (IVEN[1] > 20000) IC=1;
	if (IVEN[18] > 20000) IC=18;
	if (IC != 0) IVEN[IC]=0;
	if ((ITST[2] < 100) && (IC == 0))
		goto MMSD_90000;
MMSD_80010:
	printf("\nDO YOU WANT ME TO REINCARNATE YOU?  ");
	if (IC == 0)
		printf("\nIT WILL COST YOU 100 POINTS?  ");
	else {
		printf("\nIT WILL COST YOU NO POINTS SINCE YOU HAD THE ");
		if (IC == 1)
			printf("RING.");
		if (IC == 18)
			printf("TALISMAN.");
	}
	IANS = getchar();
	if (IANS > 'a')
		IANS -= ' ';
	if (IANS == 'N')
		goto MMSD_90020;
	if (IANS == 'Y')
		goto MMSD_80050;
	printf("\nWHAT? PLEASE ANSWER YES OR NO");
	goto MMSD_80010;

MMSD_80050:
	if (IVEN[4] > 20000) 
		IVEN[4] = IVEN[4]-20000+193;
	if ((IVEN[4] == 493) && (ITST[5] > 295))
		IVEN[4] = 1493;
	for (J=1; J<=52; J++) 
		if (IVEN[J] > 20000)
			IVEN[J] = IVEN[J]-20000+ITST[1];
	if (IRES[16] != 0)
		IRES[16] = IRES[16]-IFSD(IRES[16])+95;
	ITST[1] = 93;
	if (IC == 0) {
		ITST[2] -= 100;
		ITST[3] -= 100;
	}
	printf("\nOKAY. I MOVED YOU OUT OF HARM. ENTER LOOK TO SEE "
		"WHERE YOU ARE.");

	goto MMSB;

MMSD_90000:
	printf("\nYOU DON'T HAVE ENOUGH POINTS TO REINCARNATE");
MMSD_90020:
	printf("\n\nYOU SCORED %d POINTS WHICH RATES YOU AS A ", ITST[2]);
	for (L=0; L<8; L++) 
		putchar(ICLS[1+ITST[2]/85][L]);	
	printf("\nSLEUTH.");
	ourtime(ITIM);
	ITR=(int)(60*ITIM[4]+ITIM[3]-60*(ITST[19]/100)-IFSD(ITST[19]));
	if (ITR < 0)
		ITR += 1440;
	TG = ITST[5]/25.00;
	if (ITR/ITST[5] <= 163)
		goto MMSD_90031;
	IEF = 99;
	goto MMSD_90037;
MMSD_90031:
	IEF=200*ITR/ITST[5];
	if (IEF > 99)
		IEF=99;
MMSD_90037:
	printf("\n\nYOU PLAYED %d MINUTES REAL TIME AND %.1f HOURS GAME "
		"TIME OR %d %% UTILIZATION.", ITR, TG, IEF);
	if ((IRIT != 192) && (ITST[2] < 700))
		printf("\nBETTER LUCK NEXT TIME.");
	if (IRIT == 192)
		printf("\nHOPE YOU WILL PLAY AGAIN REAL SOON.");
MMSD_90060:
	printf("\nBYE BYE\n");
MMSD_99999:
	pak();
	exit(0);

MMSE:
	IR = IPR[2];
	IFC = ITST[4]/1000;
	IDN = 1;
	IDE = 2;
	IDS = 3;
	IDW = 4;

	if (IVEN[6] < 20000) IDN = 10-IFC;
	if (IDN == 6) IDN=10;
	if (IVEN[6] < 20000) IDE = 11-IFC;
	if (IVEN[6] < 20000) IDS = 12-IFC;
	if (IDS == 11) IDS=7;
	if (IVEN[6] < 20000) IDW = 13-IFC;
	if (IDW > 10) IDW -= 4;
	
	putchar('\n');
	putchar('\n');
	if ((ITST[5] > 450) && (IR < 84)) {
		printf("\nEVERYTHING IS IN RUIN AFTER THE EXPLOSION AND "
			"CAVE-IN. SMOKE IS EVERYWHERE\nAND GETTING THICKER. "
			"SOME THINGS ARE STILL CRASHING DOWN AROUND YOU.");
		goto MMSE_2000;
	}
	if (IR == 1) {
		printf("THE ROOM IS FURNISHED WITH JUST A COT, A STOOL AND "
			"A BENCH WITH A\nBUNCH OF BANANAS ON IT. THERE IS A "
			"PET MONKEY TIED IN ONE CORNER.");
		if (IVEN[46] != 1101)
			printf("\nTHERE IS AN EMPTY HOOK FOR THE DUNGEON "
				"KEY ON THE WALL.");
		else
			printf("\nTHERE IS A DUNGEON KEY HANGING FROM A "
				"HOOK ON THE WALL.");
		printf("\nIF YOU CANNOT FIND ANY OTHER WAY OUT YOU MIGHT "
			"BE ABLE TO GO ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDE][I]);
		printf("\nOUT THE DOOR.");
	}
	if (IR == 2) {
		printf("THE FURNACE IS WARM AND THERE IS PLENTY OF COAL "
			"NEARBY.\nTHERE IS SOOT EVERYWHERE WITH FOOT PRINTS "
			"GOING ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDE][I]);
		printf("\nACROSS THE ROOM FROM DOOR TO DOOR AND ALSO GOING ");
		for (J=0; J<8; J++)
			putchar(IDTN[IDN][J]);
		printf("\nOUT ANOTHER DOOR.");
	}
	if (IR == 3) {
		printf("THERE IS A STAIRWAY LEADING UP AND SOME WELL WORN "
			"STEPS LEADING DOWN INTO\nTHE DARKNESS WHERE YOU CAN "
			"HEAR STRANGE SOUNDS. THERE IS ALSO A DOOR LEADING\n");
		for (I=0; I<8; I++)
			putchar(IDTN[IDW][I]);
	}
	if (IR == 4) {
		printf("THERE IS AN AWKWARD STAIRWAY LEADING UP TO AT LEAST "
			"TWO OTHER LEVELS.\nYOU CAN GO ANY DIRECTION BUT "
			"DOWN HERE");
	}
	if (IR == 5) {
		printf("THERE ARE MANY COFFINS HERE CONTAINING THE "
			"REMAINS OF THE ANCESTORS OF\nMYSTERY MANSION. ALL "
			"THE COFFINS APPEAR TO BE UNDISTURBED AND SEALED "
			"EXCEPT\nFOR ONE. UNLESS YOU CAN OPEN A SECRET PANEL, "
			"YOU HAVE TO GO ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDS][I]);
		printf("\nTO GET OUT OF HERE.");
	}
	if (IR == 6) {
		printf("THE WINE RACKS ARE COMPLETELY EMPTY, PROBABLY "
			"FINISHED BY PREVIOUS PLAYERS.\nALL THAT IS LEFT "
			"IS AN EMPTY BARREL AND BROKEN BOTTLES. A DOOR GOING ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDN][I]);
		printf("\nIS THE ONLY WAY OUT OF HERE.");
	}
	if (IR == 7) {
		printf("THE ROOM IS CLUTTERED WITH MOSTLY USELESS JUNK. "
			"THERE IS A LARGE INTERESTING\nLOOKING CRATE HERE. "
			"YOU CAN GO UP A WELL WORN WOOD LADDER OR ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDE][I]);
		printf("\nTHROUGH A HEAVY WOOD DOOR WITH A SMALL WINDOW WITH "
			"IRON BARS.");
		if (IXT[320] != 0)
			printf("\nTHERE ARE ALSO SOME STEPS CUT IN THE ROCK "
				"LEADING DOWN.");
		else
			printf("\nA CAVE-IN BLOCKS THE STEPS LEADING DOWN.");
	}
	if (IR == 8) {
		printf("THE SKELETAL REMAINS OF TWO FORMER PLAYERS HANG BY "
			"SHACKLES ON THE FAR\nWALL. BETWEEN THEM IS AN EMPTY "
			"SET OF SHACKLES. YOU CAN HEAR THE SQUEAKING OF\n"
			"SEVERAL RATS AS THEY SCURRY ABOUT. THE ONLY WAY OUT "
			"IS ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDW][I]);
		printf("\nWHERE YOU CAN HEAR SOMEONE ON THE OTHER SIDE OF "
			"THE DOOR THEY JUST CLOSED");
		if (IXT[184] == 0)
			printf(" AND LOCKED.");
	}
	if (IR == 9) {
		printf("IT LOOKS LIKE THE RATS HAVE GOTTEN INTO MOST OF THE "
			"FOOD. YOU CAN GO ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDS][I]);
		printf("\nTHROUGH A DOOR LABELED WINE CELLAR OR UP THE "
			"LADDER YOU CAME DOWN FROM THE\nKITCHEN.");
	}
	if (IR == 10) {
		printf("IT IS A LARGE AND COMFORTABLE ROOM, WITH A LARGE "
			"OIL LAMP ON A STAND IN\nONE CORNER. THERE IS A LARGE "
			"PICTURE ON ONE WALL. SEEMS LIKE GOING ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDE][I]);
		printf("\nIS THE ONLY WAY OUT.");
	}
	if (IR == 11) {
		printf("THERE IS AN OLD CRANK TYPE TELEPHONE ON THE WALL "
			"BY THE FRONT DOOR.\nA LARGE WINDOW OVER LOOKS THE "
			"GROUNDS IN FRONT OF THE MANSION. THERE\nARE FOUR "
			"DOORS SO THAT YOU CAN GO LEFT, RIGHT, FORWARD, "
			"OR BACKWARD.");
	}
	if (IR == 12) {
		printf("THE ROOM IS DILAPIDATED AND HAS THE REMAINS OF "
			"VARIOUS OLD STYLE GAMES SUCH\nAS DARTS, DUCKPINS, "
			"SEVERAL CARD GAMES AND AN OLD RADIO. THERE IS A "
			"STAIRWAY\nGOING UP AND DOWN, AS WELL AS A DOOR YOU "
			"CAN EXIT BY GOING ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDW][I]);
	}
	if (IR == 13) {
		printf("THERE IS AN AWKWARD STAIRWAY LEADING UP AND DOWN TO "
			"OTHER LEVELS.\nYOU CAN GO IN ANY DIRECTION BUT ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDW][I]);
	}
	if (IR == 14) {
		printf("THE ROOM IS HUGE AND EMPTY. THE SLIGHTEST SOUND "
			"ECHOES THROUGHOUT THE\nHALL. A LARGE FIREPLACE WITH "
			"SOME FIREWOOD BY IT IS THE ONLY THING OF\nINTEREST. "
			"THERE IS A STAIRWAY LEADING UP AS WELL AS A DOOR "
			"ON EACH SIDE\nOF THE ROOM EXCEPT FOR ONE GOING ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDW][I]);
	}
	if (IR == 15) {
		printf("YOU HAVE TO GO AROUND ROWS OF TABLES TO GET "
			"ANYWHERE. THERE\nARE HUNDREDS OF WOODEN INDIANS, WITH "
			"DRAWN BOWS, STANDING ON\nPLATFORMS MOUNTED HIGH ON "
			"THE WALLS ALL AROUND THE HALL.\nTHERE ARE TWO "
			"INTERIOR DOORS YOU CAN USE BY GOING ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDN][I]);
		printf("\nOR ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDW][I]);
	}
	IC = 9 - ITST[6]/3600;
	if (IR == 16) {
		printf("THERE IS AN ALTAR IN FRONT OF THE CHAPEL WITH SOME "
			"COMMUNION WINE ON IT.");
		if (IVEN[19] == 1216)
			printf("\nTHERE IS ALSO A BURNING CANDLE ON THE ALTAR "
				"ABOUT %d INCHES LONG.", IC);
		if (IVEN[5] == 1316)
			printf("\nA SILVER CROSS ABOUT A FOOT LONG HANGS ON "
				"THE WALL BEHIND THE ALTAR.");
		else
			printf("\nYOU CAN SEE THE OUTLINE OF THE CROSS THAT "
				"USE TO HANG ON THE WALL.");
		printf("\nTHERE IS A STAIRWELL LEADING DOWN INTO THE DARKNESS "
			"AND A WAY TO GO ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDE][I]);
		printf("\nTHROUGH AN OPEN ARCH WHERE YOU CAN SEE PART OF A "
			"LIBRARY.");
	}
	if (IR == 17) {
		if (IVEN[88] != 1017)
			printf("YOU ARE STANDING NEXT TO A SMALL STAND "
				"WITH AN OLD PAPYRUS SCROLL ON IT.");
		else
			printf("YOU ARE NEXT TO THE STAND WITH THE "
				"CRUMBLED REMAINS OF THE SCROLL ON IT.");
		printf("\nTHERE ARE ROWS OF BOOKSHELVES WHERE SOMEONE COULD "
			"HIDE. YOU CAN EXIT ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDW][I]);
		printf("\nTHROUGH AN OPEN ARCH WHERE YOU CAN SEE PART OF A "
			"CHAPEL, OR ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDN][I]);
		printf("\nTHROUGH ONE OF THE TWO MATCHING DOORS AT BOTH "
			"ENDS OF THE LIBRARY.");
	}
	if (IR == 18) {
		printf("IT LOOKS LIKE A GALLEY DESIGNED TO FIX MEALS FOR A "
			"LARGE GROUP OF PEOPLE.\nTHERE IS A LADDER GOING DOWN "
			"AND A DOOR YOU CAN EXIT BY GOING ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDS][I]);
	}
	if (IR == 19) {
		printf("THIS IS A SMALL CONFINING CLOSET. IT SEEMS YOU CAN "
			"ONLY GO ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDE][I]);
		printf("\nTHROUGH THE DOOR WHICH OPENS BY ITSELF WHEN "
			"NOT LOCKED. THE WALLS,\nTHE FLOORS AND EVEN THE "
			"CEILING LOOK LIKE THEY MIGHT HAVE A SECRET\nPANEL OR "
			"A WAY THROUGH THEM.");
	}
	if (IR == 20) {
		printf("THE ROOM IS VERY MASCULINE. THERE ARE SEVERAL LARGE "
			"ANIMAL HEADS\nMOUNTED ON THE WALLS AS WELL AS OTHER "
			"THINGS. YOU CAN GO ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDW][I]);
		printf("\nINTO A SMALL CLOSET, OR YOU CAN EXIT EITHER "
			"BY GOING ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDN][I]);
		printf("\nTO THE HALLWAY OR OUTSIDE ON THE FIRE ESCAPE BY "
			"GOING ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDS][I]);
			
	}
	if (IR == 21) {
		printf("THERE IS A LARGE BELL IN THE CENTER OF THE TOWER "
			"WITH A KNOTTED CORD\nATTACHED TO THE CLAPPER THAT "
			"YOU CAN'T REACH. YOU CAN GO ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDN][I]);
		printf("\nTHROUGH A LONG SLOPING PASSAGE, OR DOWN A "
			"CIRCULAR STAIRWAY, OR ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDE][I]);
		printf("\nOUT A LARGE WINDOW OVERLOOKING THE OCEAN BELOW.");
	}
	if (IR == 22) {
		printf("THERE IS AN AWKWARD STAIRWAY LEADING DOWN TO AT "
			"LEAST TWO OTHER LEVELS.\nYOU CAN GO IN ANY DIRECTION "
			"BUT UP OR ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDW][I]);
	}
	if (IR == 23) {
		printf("THERE ARE MANY PILES OF DIRT ON THE FLOOR. YOU "
			"COULD GO IN ANY\nDIRECTION BUT UP. IT IS A VERY LONG "
			"HALLWAY WITH A SMALL WINDOW\nAT THE FAR END YOU "
			"CAN SEE BY LOOKING ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDN][I]);
	}
	if (IR == 24) {
		printf("THIS IS THE BEDROOM OF AN OLDER WOMAN, WITH "
			"A SEWING MACHINE, A LARGE\nMIRROR AND A DRESS FORM, "
			"AMONG OTHER THINGS. YOU CAN GO ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDN][I]);
		printf("\nINTO A CLOSET, OR YOU CAN EXIT THE ROOM BY GOING "
			"EITHER ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDW][I]);
		printf("\nOR ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDS][I]);
	}
	if (IR == 25) {
		printf("THE ROOM IS VERY FEMININE WITH FRILLY CURTAINS. "
			"RIGHT IN FRONT OF");
		if (IVEN[30] != 325)
			printf("\nYOU IS A SOFT COMFORTABLE CANOPY BED WITH "
				"A SMALL INDENTATION RIGHT");
		else if (IVEN[30] == 325)
			printf("\nYOU IS A SOFT COMFORTABLE CANOPY BED WITH "
				"THE GLOBE RESTING GENTLY");
		printf("\nIN THE MIDDLE. THE ONLY WAY YOU CAN SEE TO EXIT IS "
			"GOING ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDE][I]);
	}
	if (IR == 26) {
		printf("THERE IS A SMALL WINDOW HERE BUT IT WOULD GET "
			"DARK IF YOU WERE TO GO ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDS][I]);
		printf("\nTO THE OTHER END OF THE HALL. YOU CAN ENTER A "
			"DOOR BY GOING ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDW][I]);
		printf("\nOR ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDE][I]);
	}
	if ((IR == 27) || (IR == 88)) {
		printf("THIS CLOSET IS LARGE AND FEATURELESS, EXCEPT FOR "
			"A PILE OF RAGS IN ONE\nCORNER AND A SMALL DOOR IN "
			"THE WALL ON THE LEFT AS YOU ENTERED. YOU\nCAN SEE "
			"NO OTHER WAY OUT EXCEPT THE DOOR GOING ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDS][I]);
	}
	if ((IR > 27) && (IR < 35)) {
		printf("YOU ARE IN A MAZE OF TWISTY LITTLE PASSAGES. THE "
			"PASSAGES ARE LOW\nAND YOU HAVE TO STOOP TO GET "
			"AROUND. THERE IS EVIDENCE THAT SOMEBODY OR\nSOMETHING "
			"SPENDS A LOT OF TIME HERE.");
	}
	if ((IR == 35) && (ITST[17] < 10)) {
		printf("THE HIGHWAY GOES STRAIGHT SOUTH AS FAR AS YOU CAN "
			"SEE BETWEEN THE DENSE\nFOREST TO THE WEST AND A 500 "
			"FOOT SHEER CLIFF TO THE EAST.");
	}
	IC=ITST[17]*200;
	if ((IR == 35) && (ITST[17] < 5)) 
		printf("\nTHE MANSION GATE IS ABOUT %d YARDS TO THE NORTH.",IC);
	if ((IR == 35) && (ITST[17] > 4) && (ITST[17] < 10)) 
		printf("\nYOU CAN JUST SEE THE GATE TO THE NORTH.");
	if ((IR == 35) && (ITST[17] >= 10))
		printf("\nIT GOES STRAIGHT NORTH AND SOUTH AS FAR AS YOU "
			"CAN SEE.");
	if (IR == 36) 
		printf("THE WALLS AND FLOOR ARE DAMP AND COVERED WITH A "
			"SLIMY KIND OF MOSS MAKING\nIT SLIPPERY AND SLOW "
			"GOING. THE WALL TO THE LEFT AS YOU LOOK DOWN THE "
			"CORRIDOR\nSEEMS TO BE MOVING TOWARD YOU AND THERE "
			"IS A DOOR TO THE RIGHT. A COLD\nBREEZE BLOWS OUT "
			"INTO THE SECRET PASSAGE.");
	if (IR == 37)
		printf("ALMOST ALL YOU CAN SEE ARE THE SLIMY WALLS AND "
			"FLOOR. THE CORRIDOR QUICKLY\nTURNS TO A GLOOMY "
			"DARKNESS GOING DOWN BUT YOU CAN SEE THE SECRET "
			"PASSAGE. IT\nIS QUIET EXCEPT FOR AN OCCASIONAL "
			"DROP OF WATER DRIPPING FROM THE CEILING.");
	if (IR == 38)
		printf("YOU MUST BE OVER HALFWAY DOWN THE CORRIDOR "
			"BECAUSE YOU CAN BARELY SEE A\n LADDER AT THE LOWER "
			"END AND THE SECRET PASSAGE AT THE OTHER END.");
	if (IR == 39)
		printf("YOU ARE AT THE LOWER END OF THE CORRIDOR. A LADDER "
			"LEADS UP AND DOWN.\nTHE RUNGS OF THE LADDER ARE "
			"COVERED WITH THE SAME SLIMY MOSS.");
	if (IR == 40)
		printf("YOU ARE IN A LONG PASSAGE PARRALLEL TO THE "
			"CORRIDOR. THE FLOOR IS CLEAN\nAND DRY HERE, MAKING "
			"IT EASY TO GET AROUND; ESPECIALLY TO THE OTHER END.");
	if ((IR == 40) && (IXT[140] != 0)) {
		printf("\nYOU CAN GO DOWN A LADDER AT THE OTHER END OR "
			"YOU CAN GO ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDS][I]);
		printf("\nOUT THE DOOR NEXT TO YOU.");
	}
	if ((IR == 40) && (IXT[140] == 0)) 
		printf("\nTHE CORRIDOR WALLS HAVE CLOSED AND BLOCKED ALL THE "
			"EXITS.");
	if (IR == 41) {
		printf("YOU ARE IN A LARGE TREASURE ROOM. IT LOOKS LIKE "
			"THERE USED TO BE A LOT\nOF VALUABLE ARTICLES HERE "
			"ONCE. YOU NOTICE A SMALL CAVITY IN THE MIDDLE\nOF "
			"A DRAWING ON THE WALL. THERE IS A LADDER GOING UP, "
			"WHICH IS ALL SLIMY.");
		if (IXT[320] == 0)
			printf(" THERE ARE STEPS CARVED IN THE SIDES OF A "
				"HUGE SHAFT LEADING DOWN.");
	}
	if ((IR > 41) && (IR < 52) && (IXT[320] != 0)) {
		printf("YOU ARE IN A MAZE OF LARGE PASSAGES. SOME OF THE "
			"PASSAGES LOOK\nFRESHLY DUG. THERE ARE LARGE ODD "
			"LOOKING FOOT PRINTS EVERYWHERE\nAND A FEW BONES "
			"HERE AND THERE.");
	}
	if ((IR > 41) && (IR < 51) && (IXT[320] == 0)) {
		printf("YOU ARE IN A CAVERN DEEP BELOW THE MANSION. A LOT "
			"OF WORK HAS\nGONE INTO CARVING STEPS AND TUNNELS IN "
			"THE ROCK WALLS AND FLOOR.\nTHERE ARE LOOSE ROCKS "
			"EVERYWHERE.");
	}
	if ((IR == 52) && (IXT[320] != 0)) {
		printf("THERE ARE MANY BONES SCATTERED ABOUT, SOME OF WHICH "
			"LOOK HUMAN.\nTHERE IS A MESSAGE WRITTEN IN BLOOD ON "
			"ONE WALL THAT SAYS THE\nMURDER WAS COMMITTED WITH "
			"THE %s", IWP);
	}
	if ((IR > 50) && (IR < 54) && (IXT[320] == 0)) {
		printf("THE ROOM WAS CARVED OUT OF SOLID ROCK A LONG LONG "
			"TIME AGO.\n");
	}
	if (((IR == 51) || (IR == 53)) && (IXT[320] == 0)) {
		printf("THERE IS A TROLL SLEEPING FITFULLY IN ONE CORNER "
			"AND IS OBVIOUSLY\nTOO LARGE TO FIT DOWN THE ONLY "
			"EXIT IN THE MIDDLE OF THE ROOM.\n");
	}
	if ((IR == 51) && (IVEN[42] == 1151)) {
		printf("IT HAS A RUBY NECKLACE DANGLING FROM ONE HAND AND A "
			"BLUDGEON IN THE OTHER.\n");
	}
	if ((IR == 51) && (IVEN[40] == 1151)) {
		printf("NEXT TO THE TROLL IS AN ANIMATED STATUE OF A "
			"BEAUTIFUL NAKED WOMAN THAT\nIS PART BIRD AND IS "
			"SINGING PEACEFULLY AND SEDUCTIVELY WHILE SHE "
			"ENTICES\nYOU WITH SEVERAL DIAMONDS CUPPED IN "
			"HER OUTSTRETCHED HANDS.\n");
	}
	if ((IR == 51) && (IXT[320] == 0)) {
		printf("ACROSS FROM THE TROLL IS A GOLD FIGURINE OF A "
			"BULL ON A PILE OF GOLD BARS.");
	}
	if ((IR == 53) && (IVEN[39] == 1253)) {
		printf("IT HAS A SILVER GOBLET RESTING IN ONE HAND AND A "
			"BLUDGEON IN THE OTHER.\n");
	}
	if ((IR == 53) && (IVEN[37] == 1153)) {
		printf("NEXT TO THE TROLL IS A STONE IDOL WITH A LARGE "
			"EMERALD FOR ONE EYE.\n");
	}
	if ((IR == 53) && (IXT[320] == 0)) {
		printf("ACROSS FROM THE TROLL IS A SACRIFICIAL STONE SLAB "
			"WITH A DAGGER IN IT.");
	}
	if ((IR == 53) && (IXT[320] != 0)) {
		printf("THIS CAVE IS ABOVE THE DOMAIN OF THE GIANT MOLE. AS "
			"LONG AS YOU ARE\nNEAR, HE WILL CONSTANTLY CHANGE HIS "
			"TUNNELS TRYING TO CONFUSE YOU IF HE CAN.\nTHERE IS A "
			"PATH LEADING UP AND DOWN.");
	}
	if ((IR == 52) && (IXT[320] == 0)) {
		printf("IT IS EMPTY EXCEPT FOR SEVERAL BOULDERS AND SOME "
			"SKELETONS COVERED WITH\nCRABS. THE FLOOR IS UNEVEN, "
			"BEING FULL OF CRATERS.\n");
	}
	if ((IR == 52) && (IXT[320] == 0) && (IXT[319] != 0)) {
		printf("THE ONLY WAY OUT IS DOWN THE TUNNEL THAT IS IN THE "
			"MIDDLE OF THE DEN.");
	}
	if (IR == 54) {
		printf("A LARGE GARDEN WITH HIGH HEDGES STRETCHES AROUND "
			"YOU. THE MAIN PATH\nGOES NORTH WITH A ONEWAY EXIT "
			"GATE ON EACH SIDE. A SIGN SAYS THAT\nTHE GARDEN "
			"CLOSES AT SUNSET FOR YOUR SAFETY.");
	}
	if (IR == 55) {
		printf("A SIGN SAYS: DON'T PICK THE FLOWERS, NO SWIMMING AND "
			"STAY OFF THE GRASS.\nYOU CAN GO SOUTH TO THE VERANDA "
			"OR EAST OR WEST.");
	}
	if ((IR == 56) || (IR == 57)) {
		printf("A SIGN SAYS: HOPE YOU ENJOYED YOUR VISIT. YOU CAN GO "
			"NORTH INTO THE GARDEN\nOR YOU CAN GO ONTO THE "
			"VERANDA.");
	}
	if ((IR >= 58) && (IR <= 63)) {
		printf("HIGH HEDGES BLOCK THE VIEW AROUND YOU BUT YOU "
			"CAN SEE SOME OF THE\nGARGOYLES ALONG THE ROOF OF "
			"THE MANSION. THERE ARE PATHS GOING IN SEVERAL\n"
			"DIRECTIONS.");
	}
	if ((IR == 64) || (IR == 65)) {
		printf("YOU ARE ON THE SOUTHERN EDGE OF A GRASSY MEADOW.");
	}
	if (IR == 66) {
		printf("YOU ARE ON THE WESTERN EDGE OF THE GRASSY MEADOW.");
	}
	if (IR == 67) {
		printf("YOU ARE ON THE EASTERN EDGE OF THE GRASSY MEADOW. "
			"THE WALL\nIS TO THE EAST BUT PATHS GO EVERY OTHER "
			"WAY.");
	}
	if (IR == 68) {
		printf("YOU ARE ON THE NORTHERN EDGE OF THE GRASSY MEADOW "
			"AND THE WESTERN EDGE\nOF A POND. AS YOU LOOK UP "
			"YOU CAN SEE THE WORD ");
		for (L=0; L<6; L++)
			putchar(IVRB[7][L]);
		printf("WRITTEN UNDERNEATH\nA BRIDGE CROSSING THE POND.");
	}
	if (IR == 69) {
		printf("YOU ARE ON THE NORTHERN EDGE OF A GRASSY MEADOW AND "
			"THE\nEASTERN EDGE OF A POND. AS YOU LOOK UP YOU "
			"CAN SEE THE WORD\n");
		for (L=0; L<6; L++)
			putchar(IVRB[11][L]);
		printf("WRITTEN UNDERNEATH A BRIDGE CROSSING THE POND.");
	}
	if (IR == 70) {
		printf("YOU ARE ON THE NORTHERN SHORE OF THE POND. THE WALL "
			"IS TO THE NORTH.");
	}
	if ((IR == 71) || (IR == 72)) {
		printf("THE GRASS IS TALL AND THERE ARE MANY BUSHES AROUND.");
	}
	if ((IR > 63) && (IR < 73) && (IR != 67) && (IR != 70))
		printf("\nYOU CAN GO IN ANY DIRECTION FROM HERE.");
	if ((IR == 73) || (IR == 74)) 
		printf("THE FOUNTAIN HAS NOT BEEN USED IN YEARS AND IS DRY. "
			"THERE IS A PATH IN\nEACH DIRECTION BUT NORTH.");
	if (IR == 75)
		printf("THE WELL IS DEEP AND DARK. PATHS GO EAST AND WEST.");
	if (IR == 76)
		printf("YOU ARE ON THE EASTERN EDGE OF A GRASSY MEADOW. "
			"PATHS GO NORTH AND WEST.");
	if (IR == 77)
		printf("YOU ARE ON THE WESTERN EDGE OF A GRASSY MEADOW. "
			"PATHS GO NORTH AND EAST.");
	if ((IRES[15] < 10000) && (((IR > 63) && (IR < 70)) ||
	                           ((IR == 76)|| (IR ==77)))) {
		printf("\nAS YOU LOOK OUT OVER THE GRASSY MEADOW YOU CAN SEE "
			"SOMETHING MOVING\nTOWARD YOU IN THE GRASS.");
	}
	if ((IR == 78) || (IR == 80)) 
		printf("THE WALLS ARE SLIPPERY AND YOU CANNOT GET OUT.");
	if (IR == 79)
		printf("IT IS DRY HERE. THE WALLS ARE MADE WITH UNEVEN "
			"BRICKS YOU CAN EASILY CLIMB.");
	if ((IR == 81) || (IR == 82)) {
		printf("YOU ARE OVER A SMALL POND. AS YOU CROSS YOU CAN "
			"SEE THE\nWORD ");
		if (IR == 81) for (L=0; L<6; L++)
			putchar(IVRB[10][L]);
		if (IR == 82) for (L=0; L<6; L++)
			putchar(IVRB[6][L]);
		printf("REFLECTED IN THE WATER. THE BRIDGE GOES NORTH AND "
			"SOUTH.");
	}
	if (IR == 83) {
		printf("YOU ARE WAIST DEEP IN WATER. YOU CAN SEE TWO "
			"BRIDGES WITH THE WORDS\n");
		for (L=0; L<6; L++)
			putchar(IVRB[8][L]);
		printf("AND ");
		for (L=0; L<6; L++)
			putchar(IVRB[4][L]);
		printf("WRITTEN UNDERNEATH THEM. THE BANKS ARE STEEP AND "
			"YOU\nCANNOT GET OUT.");
	}
	if (IR == 84) {
		printf("YOU ARE STANDING IN THE MIDDLE OF A SMALL COTTAGE ON "
			"A PLATEAU ON\nTHE SIDE OF THE CANYON. AS YOU LOOK "
			"AROUND, YOU CAN SEE A\n");
		if (IVEN[28] == 1184)
			printf("SMALL ROAD MAP LIGHTLY TACKED TO ONE WALL. "
				"ALSO, THERE ARE SOME\n");
		else
			printf("MESSAGE ON THE WALL WHERE THE MAP USED TO BE "
				"THAT SAYS: GO CRAZY\nIN THE WOODS TO GET "
				"QUICK ASYLUM HERE. ALSO, THERE ARE SOME\n");
		printf("STEPS GOING OUT THE BACK DOOR TO A PATH CONTINUING "
			"UP THE\nEMBANKMENT. THERE IS ALSO THE WAY YOU CAME "
			"IN, BUT THAT JUST\nLEADS BACK DOWN TO THE STREAM BY "
			"THE WOODS.");
	}
	if ((IR == 85) || (IR == 86)) {
		printf("IT IS MADE OF RUSTY WROUGHT IRON. THE STEPS ARE "
			"HINGED SO THAT ONCE\nYOU GO DOWN YOU CANNOT GO BACK "
			"UP. BUT THE HINGE HAS A HABIT OF RUSTING\nSO IT "
			"CANNOT MOVE.");
		if (IR == 86) {
			printf(" THE LADDER IN THE ATTIC WAS DESTROYED "
				"WHEN YOU CLIMBED\nUP IT AND YOU CANNOT "
				"GET BACK DOWN IT.");
			IXT[265] = 0;
			ITST[14] = 86;
		}
	}
	if (IR == 87) {
		printf("YOU ARE IN A DUSTY ROOM THICK WITH COBWEBS. "
			"YOU MUST STOOP TO GET AROUND.");
		if (ITST[5] < 300) 
			printf("\nA SMALL GRATE LETS IN JUST ENOUGH LIGHT "
				"TO SEE.");
		if (IXT[265] != 0)
			printf("\nTHERE IS A HALF ROTTED LADDER LEADING UP "
				"TO A FIRE ESCAPE.");
	}
	if (IR == 89)
		printf("IT IS THE LABORATORY OF THE MAD SCIENTIST. THERE ARE "
			"SEVERAL PIECES OF\nEQUIPMENT HERE WHICH ARE "
			"ALL HUMMING READY TO WORK. IT SORT OF LOOKS LIKE\n"
			"THE TRANSPORTER ROOM OUT OF A STAR TREK MOVIE. LARGE "
			"WINDOWS OVERLOOK THE\nGROUNDS. THERE IS A DOOR TO "
			"THE SOUTH AND A SMALL DOOR ON THE FLOOR.");
	if (IR == 90) {
		printf("YOU CAN SEE ALL THE USUAL FIXTURES; ESPECIALLY "
			"A VERY NICE SHOWER.\nTHERE IS A SMALL DOOR IN THE "
			"WALL IN FRONT OF YOU AS YOU ENTERED AND\nANOTHER "
			"SMALL DOOR IN THE CEILING. A SMALL WINDOW OVERLOOKS "
			"THE\nGROUNDS FAR BELOW.  SEEMS THE ONLY OTHER WAY "
			"OUT IS BY GOING ");
		for (I=0; I<8; I++)
			putchar(IDTN[IDW][I]);
	}
	if (IR == 91) 
		printf("YOU ARE ON A SCREENED IN PORCH IN FRONT OF THE "
			"MANSION. THERE IS\nA SMALL PLAQUE ON THE DOOR WITH "
			"WRITING ON IT. YOU CAN EITHER\nGO NORTH INTO THE "
			"MANSION OR GO WEST TO THE DRIVEWAY.");
	if (IR == 92)
		printf("YOU ARE ON A LONG DRIVE THAT LEADS EASTERLY FROM "
			"THE GATE TO THE\nMANSION. A SIGN ALONG THE DRIVEWAY "
			"READS: STOP LOOK AND LISTEN.\nTHE WALL IS TO THE "
			"SOUTH AND TREES LINE THE NORTHERN SIDE.");
	if (IR == 93) {
		printf("YOU ARE SOUTH OF THE HEAVY IRON GATE. A SIGN SAYS "
			"ENTER AT YOUR OWN RISK.");
		if (IVEN[4] == 493) 
			printf("\nTHERE IS A SHINY BRASS LANTERN HANGING "
				"ON THE GATE POST BY THE SIGN.");
		if (IVEN[4] == 1493) 
			printf("\nTHERE IS A BRIGHTLY SHINING LANTERN HANGING "
				"ON ON A BRASS HOOK NEXT TO THE SIGN.");
		if ((IVEN[4] != 493)  && (IVEN[4] != 1493)) 
			printf("\nTHERE IS AN EMPTY SHINY BRASS HOOK ON THE "
				"GATE POST NEXT TO THE SIGN.");
		printf("\nYOU CAN SEE THE ROAD TURN NORTH ALONG THE "
			"WALL ABOUT A QUARTER MILE TO\nTHE EAST AND TO THE "
			"WEST. YOU ARE ALSO AT THE END OF THE HIGHWAY\n"
			"WHICH GOES SOUTH TO THE BIG CITY.");
	}
	if (IR == 94)
		printf("YOU ARE OUTSIDE THE WESTERN WALL OF THE MANSION. "
			"THE ROAD ENDS HERE\nBUT THE WALL CONTINUES NORTH "
			"INTO A DENSE FOREST. THE FOREST IS\nALSO TO THE WEST "
			"OF THE ROAD BUT HAS BEEN PARTIALLY CLEARED. YOU\nCAN "
			"SEE THE ROAD TURN EAST ABOUT A QUARTER MILE TO THE "
			"SOUTH.");
	if (IR == 95)
		printf("YOU ARE ON THE ROAD AT THE BACK GATE OF THE "
			"MANSION. THE HINGES HAVE\nRUSTED THROUGH AND THE "
			"GATE HANGS HALF OPEN. THERE IS A PATH LEADING\n"
			"SOUTH THROUGH THE GATE AND BETWEEN TWO ROWS OF "
			"HEDGES. TO THE NORTH\nOF THE ROAD IS A 500 FOOT "
			"SHEER CLIFF, OVERLOOKING THE SEA SHORE. THE\n"
			"ROAD ENDS HERE BUT THE WALL CONTINUES WEST INTO THE "
			"DENSE FOREST.");
	if (IR == 96) /**/
		printf("YOU ARE OUTSIDE THE EASTERN WALL OF THE MANSION. "
			"THE WALL FORMS\nPART OF THE MANSION TO THE SOUTH "
			"WHERE THE ROAD GOES UNDER IT\nBEFORE TURNING WEST. "
			"THE ROAD ALSO TURNS TO THE WEST ALONG\nTHE WALL "
			"ABOUT A QUARTER MILE TO THE NORTH. THERE IS A 500\n"
			"FOOT SHEER CLIFF TO THE EAST OF THE ROAD.");
	if (IR == 97)
		printf("YOU ARE IN A LIGHTLY WOODED PART OF THE FOREST. "
			"THERE ARE NO PATHS\nAND YOU CANNOT GO STRAIGHT IN "
			"ANY DIRECTION VERY LONG.");
	if (IR == 98)
		printf("YOU ARE IN A DENSELY WOODED PART OF THE FOREST. YOU "
			"CAN NO LONGER TELL\nCARDINAL DIRECTIONS. THERE IS NO "
			"WAY TO TELL IF YOU ARE GOING\nSTRAIGHT AND YOU "
			"COULD EASILY GO IN CIRCLES AND NOT KNOW IT.");
	if (IR == 99)
		printf("THE STREAM FLOWS NORTH TO SOUTH IN A DEEP CANYON. "
			"THERE ARE\nSOME STEPS CUT IN THE ROCK THAT GO DOWN "
			"INTO A CAVE AND UP THE\nSIDE OF THE CANYON TO A "
			"COTTAGE BUILT ON A SMALL PLATEAU.");

MMSE_2000:
	goto MMSB;

MMSF:
	IR=ITST[1];
	R=RND;
	if (IWRD[0][9] <= 20) goto MMSF_1000;
	if (IWRD[0][9] == 21) goto MMSF_2000;
	if (IWRD[0][9] == 22) goto MMSF_2000;
	if (IWRD[0][9] == 23) goto MMSF_3000;
	if (IWRD[0][9] == 24) goto MMSF_4000;
	if (IWRD[0][9] == 25) goto MMSF_5000;
	if (IWRD[0][9] == 26) goto MMSF_5000;
	if (IWRD[0][9] == 27) goto MMSF_6000;
	if (IWRD[0][9] == 28) goto MMSF_7000;
	if (IWRD[0][9] == 29) goto MMSF_8000;
	if (IWRD[0][9] == 30) goto MMSF_8000;
	if (IWRD[0][9] == 31) goto MMSF_9000;
	if (IWRD[0][9] == 32) goto MMSF_10000;
	if (IWRD[0][9] == 33) goto MMSF_10000;
	if (IWRD[0][9] == 34) goto MMSF_11000;
	if (IWRD[0][9] == 35) goto MMSF_11000;
	if (IWRD[0][9] == 36) goto MMSF_12000;
MMSF_1000:
	if (((ITST[5] - (ITST[5]/25)*25) == 13) && (IWRD[0][9] == 20) &&
	    (IWRD[1][8] == 3) && (IWRD[1][9] == 71) && (ITST[5] < 450))
		ITST[2] += 2;
	if (((ITST[5]-(ITST[5]/25)*25) == 13) && (IWRD[0][9] == 20) &&
	    (IWRD[1][8] == 3) && (IWRD[1][9] == 71) && (ITST[5] < 450)) {
		IPR[2] = 1040;
		IPR[3] = 0;
		goto MMSG;	
	}
	if ((IWRD[0][9] == 20) && (IWRD[1][8] == 3) && (IWRD[1][9] == 71)) {
		IPR[2] = 520;
		IPR[3] = 0;
		goto MMSG;	
	}
	if (IWRD[0][10] > 1) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSG;	
	}
	ISC = ITST[2];
	ISCP = ITST[3];
	ISCL = (ISC/85+1)*85-ISC;
	printf("\nSO FAR YOU HAVE SCORED %d POINTS. YOU CAN STILL REACH %d "
		"POINTS", ISC, ISCP);
	if (IWRD[0][9] == 20) {		
		IPR[2] = 0;
		IPR[3] = 0;
		goto MMSB;	
	}
	printf("\nOF THE ORIGINAL 999 IF YOU CONTINUE. THIS RATES YOU AS A ");
	for (L=0; L<8; L++)
		putchar(ICLS[ISC/81+1][L]);
	printf("\nSLEUTH. YOU NEED %d MORE POINTS TO REACH THE NEXT LEVEL "
		"OF SKILL.\nENTER SCORE NEXT TIME IF THAT IS WHAT YOU WANT.",
			ISCL);
	ourtime(ITIM);
	ITR=(int)(60*ITIM[4]+ITIM[3]-60*(ITST[19]/100)-IFSD(ITST[19]));
	if (ITR < 0) ITR += 1440;
	TG = ITST[5]/25.00;
	if ((ITR/ITST[5]) <= 163) goto MMSF_1012;
	IEF=99;
	goto MMSF_1013;
MMSF_1012:
	IEF=200*ITR/ITST[5];
	if (IEF > 99) IEF=99;
MMSF_1013:
	printf("\n\nYOU PLAYED %d MINUTES REAL TIME AND %5.1f HOURS GAME "
		"TIME OR %d %% UTILIZATION.", ITR, TG, IEF);
MMSF_1021:
	printf("\n\nDO YOU REALLY WANT TO QUIT NOW?  ");
	IANS = getchar();
	if (IANS >= 'a')
		IANS -= ' ';
	if (IANS == 'N') {
		IPR[2] = 1040;
		IPR[3] = 0;
		goto MMSG;	
	}
	if (IANS == 'Y') {
		IPR[2] = 1032;
		IPR[3] = 0;
		goto MMSD;	
	}
	printf("\nWHAT? PLEASE ANSWER YES OR NO");
	goto MMSF_1021;
MMSF_2000:
	if (IWRD[0][10] == 1) {
		IPR[2] = 1000;
		IPR[3] = 0;
		goto MMSG;	
	}
	if ((IWRD[0][10] > 2) && ((IWRD[2][8] != 5) || (IWRD[2][9] != 7) ||
	    (IWRD[1][8] != 3) || (IWRD[3][8] != 4))) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSG;	
	}
	if (IWRD[1][8] != 3) {
		IPR[2] = 2011;
		IPR[3] = 0;
		goto MMSG;	
	}
	if ((IWRD[1][9] == 99) && (IWRD[0][9] == 22)) goto MMSF_2100;
	if (IVEN[IWRD[1][9]] > 20000) {
		IPR[2] = 2020;
		IPR[3] = 0;
		goto MMSG;	
	}
	if ((IWRD[0][10] > 2) && (IFSD(IVEN[IWRD[1][9]]) != IWRD[3][9]) &&
	    (IVEN[IWRD[1][9]] < 10000)) {
		IPR[2] = 2025;
		IPR[3] = 0;
		goto MMSG;	
	}
	if ((IVEN[IWRD[1][9]] == 0) && (IWRD[1][9] > 53)) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSG;	
	}
	if (((IFSD(IVEN[IWRD[1][9]])  != IR) && (IVEN[IWRD[1][9]] < 10000)) || 
            ((IVEN[IWRD[1][9]] >= 10000) && 
	    (IRES[IVEN[IWRD[1][9]]-(IVEN[IWRD[1][9]]/100)*100]-
	    (IRES[IVEN[IWRD[1][9]]-(IVEN[IWRD[1][9]]/100)*100]/100)*100!=IR))||
	    ((IVEN[IWRD[1][9]] > 10000) && (IWRD[0][9] == 21))) {
		IPR[2] = 2030;
		IPR[3] = 0;
		goto MMSG;	
	}
	if (IWRD[1][9] == 88) {
		IPR[2] = 2037;
		IPR[3] = 0;
		goto MMSG;	
	}
	if ((IWRD[1][9] > 99) && (IWRD[1][9] < 103)) {
		IPR[2] = 2041;
		IPR[3] = 0;
		goto MMSG;	
	}
	if (IWRD[1][9] == 103) {
		IPR[2] = 2042;
		IPR[3] = 0;
		goto MMSG;	
	}
	if ((IWRD[1][9] == 29) && (ITST[5] > 50) && (IVEN[29] == 1214)) {
		IPR[2] = 2035;
		IPR[3] = 0;
		goto MMSG;	
	}
	if (IWRD[1][9] > 53) {
		IPR[2] = 2040;
		IPR[3] = 0;
		goto MMSG;	
	}
	IC = ITD(IVEN[IWRD[1][9]]);
	for (J=1; J<=53; J++) {
		if (IVEN[J] < 20000) continue;
		IC = IC+ITD(IVEN[J]);
	}
	if ((IC > 16) && (IVEN[48] < 20000)) {
		IPR[2] = 2060;
		IPR[3] = 0;
		goto MMSG;	
	}
	if (IC > 24) {
		IPR[2] = 2060;
		IPR[3] = 0;
		goto MMSG;	
	}
	K = IWRD[1][9];
	if ((K != 5) && (K != 17) && (K != 29) && (K != 36) && (K != 37) &&
	    (K != 39) && (K != 40) && (K != 42) && (K != 46) && (K != 4) &&
	    (K != 28)) goto MMSF_2052;
	if ((IFD(IVEN[K]) == 1) && (K != 4)) IVEN[K] -= 1000;
	if ((ITD(IVEN[4]) == 4) && (K == 4)) IVEN[4] -= 100;
MMSF_2052:
	R=RN(R);
	if (IWRD[1][9] == 21) ITST[16]=(int)(R*89+1);
	if ((IWRD[1][9] == 21) && (IVRB[ITST[16]][0] == ' ')) goto MMSF_2052;
	IVEN[IWRD[1][9]]=(IVEN[IWRD[1][9]]/100)*100-(IVEN[IWRD[1][9]]/10000)*
		10000+20000;
	if ((ITST[5] < 100) || (ITST[5] > 400)) goto MMSF_2095;
	for (J=2; J<=14; J++) {
		if ((IRES[J] > 10000) || (ITD(IRES[J]) == 0) ||
		    (IFSD(IRES[J]) == 0)) continue;
		for (K=1; K<=53; K++) {
			if ((IVEN[K] > 10000) || 
			    (IFSD(IRES[J]) != IFSD(IVEN[K]))) continue;
			R=RN(R);
			if ((R > 0.2) && (ITST[5] < 250)) continue;
			if ((R > 0.1) && (ITST[5] > 250)) continue;
			IC=ITD(IVEN[K]);
			for (L=1; L<=53; L++) {
				if ((IVEN[L] < 10000) ||
				    (IVEN[L] > 20000) ||
				    (IFSD(IVEN[L]) != J)) continue;
				IC=IC+ITD(IVEN[L]);
			}
			R=RN(R);
			if (R > 0.3) continue;
			if (IC <= ITD(IRES[J])) 
				IVEN[K] = 10000+(IVEN[K]/100)*100+J;
			if ((IC <= ITD(IRES[J])) &&
			    (IFSD(IRES[J]) == IR)) {
				printf("\nYOU JUST SAW ");
				for (I=0; I<8; I++)
					putchar(IRSN[J][I]);
				printf("\nPICK UP ");
			}
			if ((IC <= ITD(IRES[J])) && (IFSD(IRES[J]) == IR))
				MMRI(4, K);
		}
	}
MMSF_2095:
	IPR[2] = 2070;
	IPR[3] = 0;
	goto MMSG;	
MMSF_2100:
	if (IR != 90) {
		IPR[2] = 2110;
		IPR[3] = 0;
		goto MMSG;	
	}
	for (I=1; I<=52; I++) {
		if (IVEN[I] > 20000) {
			IPR[2] = 2120;
			IPR[3] = 0;
			goto MMSG;	
		}
	}
	if (IVEN[53] > 20000) {
		IPR[2] = 2140;
		IPR[3] = 0;
		goto MMSG;	
	}
	ITST[32]=1;
	IPR[2] = 2150;
	IPR[3] = 0;
	goto MMSG;	
MMSF_3000:
	if (IWRD[0][10] == 1) {
		IPR[2] = 1000;
		IPR[3] = 0;
		goto MMSG;	
	}
	if ((IWRD[0][10] == 2) && (IWRD[1][8] == 5) && (IWRD[1][9] == 11))
		goto MMSF_3200;
	if ((IWRD[0][10] > 2) || (IWRD[1][8] != 3)) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSG;	
	}
	if (IVEN[IWRD[1][9]] < 20000) {
		IPR[2] = 3010;
		IPR[3] = 0;
		goto MMSG;	
	}
	if (IR == 35) 
		IVEN[IWRD[1][9]] = ITD(IVEN[IWRD[1][9]])+98;
	if (IR == 35) {
		IPR[2] = 3015;
		IPR[3] = 0;
		goto MMSG;	
	}
	if ((IR < 75) || (IR > 77)) goto MMSF_3100;
	for (J=326; J<=328; J++) {
		if (IFSD(IXT[J]) != IR) continue;
		IVEN[IWRD[1][9]]=IVEN[IWRD[1][9]]-20000+IXT[J]/100;
		if ((IXT[J]/100) == 79) {
			IPR[2] = 3030;
			IPR[3] = 0;
			goto MMSG;	
		}
		if ((IXT[J]/100) != 79) {
			IPR[2] = 3040;
			IPR[3] = 0;
			goto MMSG;	
		}
	}
MMSF_3100:
	if ((IWRD[1][9] == 30) && (IR != 25)) {
		IVEN[30] = 0;
		IPR[2]   = 3045;
		IPR[3]   = 0;
		goto MMSG;	
	}
	IVEN[IWRD[1][9]]=IVEN[IWRD[1][9]]-20000+IR;
	for (J=2; J<=14; J++) {
		if ((ITD(IRES[J]) == 0) || (IRES[J] > 10000)) continue;
		R=RN(R);
		if ((R > 0.1) && (ITST[5] < 250)) goto MMSF_3130;
		if ((R > 0.2) && (ITST[5] > 250)) goto MMSF_3130;
		for (K=1; K<=53; K++) {
			if ((IVEN[K] < 10000) || (IVEN[K] > 20000) ||
			    (IFSD(IVEN[K]) != J)) continue;
			R=RN(R);
			if (R > 0.3) continue;
			IVEN[K] = IVEN[K]-10000-J+IFSD(IRES[J]);
			if (IFSD(IRES[J]) == IR) {
				printf("\nYOU JUST SAW ");
				for (L=1; L<8; L++)
					putchar(IRSN[J][L]);
				printf("PUT DOWN ");
				MMRI(4, K);
			}
		}
	}
MMSF_3130:
	IPR[2] = 3110;
	IPR[3] = 0;
	goto MMSG;
MMSF_3200:
	if (IR == 35) {
		IPR[2] = 3210;
		IPR[3] = 0;
		goto MMSG;
	}
	IC=IX=0;
	for (I=1; I<=52; I++) {
		if ((I == 30) && (IVEN[I] > 20000)) IX=1;
		if (IVEN[I] < 20000) continue;
		IVEN[I]=IVEN[I]-20000+IR;
		IC=1;
	}
	if (IC == 0) {
		IPR[2] = 520;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IX == 1) && (IR != 25)) {
		IVEN[30] = 0;
		IPR[2]   = 3045;
		IPR[3]   = 0;
		goto MMSG;
	}
	/*
	 * Hmm, interesting bug here.  Clearly, this should be assigned
	 * the value 325, but my "original" source gave it as 1325, which
	 * which caused extremely whacked-out behavior.  This does not 
	 * inspire great confidence in my original source!  Fixed 2/6/2000.
	 */
	if (IX == 1) IVEN[30]=325;
	IPR[2] = 1040;
	IPR[3] = 0;
	goto MMSG;
MMSF_4000:
	if (IWRD[0][10] == 1) {
		IPR[2] = 1000;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IWRD[0][10] != 4) || (IWRD[1][8] != 3) ||
	    (IWRD[3][8] != 4) || (IWRD[2][8] != 5) ||
	    (IWRD[2][9] != 5)) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IVEN[IWRD[1][9]] < 20000) {
		IPR[2] = 3010;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IFSD(IRES[IWRD[3][9]]) != IR) {
		IPR[2] = 4030;
		IPR[3] = 0;
		goto MMSG;
	}
	IC=ITD(IVEN[IWRD[1][9]]);
	for (J=1; J<=53; J++) {
		if (((IVEN[J]/10000) != 1) || (IWRD[3][9] != IFSD(IVEN[J])))
			continue;
		IC=IC+ITD(IVEN[J]);
	}
	if (IC > ITFD(IRES[IWRD[3][9]])) {
		IPR[2] = 4030;
		IPR[3] = 0;
		goto MMSG;
	}
	IVEN[IWRD[1][9]] = IVEN[IWRD[1][9]]-10000+IWRD[3][9];
	IPR[2] = 3110;
	IPR[3] = 1;
	goto MMSG;
MMSF_5000:
	if (IWRD[0][10] == 1) {
		IPR[2] = 1000;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IWRD[0][9] == 25) && (IWRD[1][8] == 5) && 	
	    (IWRD[1][9] == 1)  && (IWRD[2][9] == 64)) IWRD[1][9]=4;
	if ((IWRD[0][9] == 26) && ((IWRD[1][8] != 3) || 	
	    ((IWRD[1][9] != 3) && (IWRD[1][9] != 6) &&
	     (IWRD[1][9] != 18) && (IWRD[1][9] != 21) &&
	     (IWRD[1][9] != 22) && (IWRD[1][9] != 28) &&
	     (IWRD[1][9] != 90) && (IWRD[1][9] != 88)))) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[0][9] == 26) {
		IWRD[2][8]=IWRD[1][8];
		IWRD[2][9]=IWRD[1][9];
		IWRD[1][8]=5;
		IWRD[1][9]=4;	
	}
	if ((IWRD[1][8] != 5) || (IWRD[1][9] != 4)) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IWRD[2][8] != 3) && (IWRD[2][8] != 4)) {
		IPR[2] = 5010;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[2][8] == 3) goto MMSD_5100;
	if (IFSD(IRES[IWRD[2][9]]) != IR) {
		IPR[2] = 5020;
		IPR[3] = 0;
		goto MMSG;
	}
	IC=0;
	printf("\nYOU ARE LOOKING AT THE ");
	for (L=0; L<8; L++) putchar(IRSN[IWRD[2][9]][L]);
	printf("\nWHO HAS:");
	MMRI(3,IWRD[2][9]);
	IPR[2] = 5070;
	IPR[3] = 0;
	goto MMSG;
MMSD_5100:
	if ((IVEN[IWRD[2][9]] < 20000) && (IWRD[2][9] < 54)) {
		IPR[2] = 5110;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IXT[320] == 0) && ((IR == 51) || (IR == 53))) IVEN[79]=IR;
	if ((IFSD(IVEN[IWRD[2][9]]) != IR) && (IWRD[2][9] > 53)) {
		IPR[2] = 8030;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[2][9] == 30) {
		IPR[2] = 5130;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[2][9] == 15) {
		IPR[2] = 5140;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[2][9] != 28) goto MMSF_5200;
	IC=ID=0;
	for (J=1; J<=360; J++) {
		if (IFSD(IXT[J]) != IR) continue;
		if ((ITFD(IXT[J]) < 92) && (ITFD(IXT[J]) != 35) &&
		    (ITFD(IXT[J]) != 0)) continue;
		IC = (J-1)/60+1;
		if (ID == 0) printf("\nTHE MAP SHOWS THAT IT IS:");
		ID=1;
		if (ITFD(IXT[J]) != 0) {
			putchar('\n');
			for (L=0; L<8; L++) putchar(IDTN[IC][L]);
			printf("TO THE ");
			for (L=0; L<16; L++) 
				putchar(IRNM[IFSD(IROM[ITFD(IXT[J])])][L]);
		}
		if (ITFD(IXT[J]) == 0) {
			for (L=0; L<8; L++) putchar(IDTN[IC][L]);
			printf("TO THE 500 FOOT SHEER CLIFF");
		}
	}
	if (IC == 0) {
		IPR[2] = 5170;
		IPR[3] = 0;
		goto MMSG;
	}
	IPR[2] = 0;
	IPR[3] = 0;
	goto MMSG;
MMSF_5200:
	if (IWRD[2][9] != 19) goto MMSF_5300;
	IWRD[2][9] = 9 - ITST[6]/3600;
	IPR[2] = 5210;
	IPR[3] = 0;
	goto MMSG;
MMSF_5300:
	if (IWRD[2][9] == 27) {
		IPR[2] = 5310;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[2][9] == 9) {
		IPR[2] = 5410;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[2][9] != 3) goto MMSF_5600;
	IHR=6+ITST[5]/25;
	if (IHR > 12) IHR -= 12;
	if (IHR > 12) IHR -= 12;
	MIN=(int)((ITST[5] - (ITST[5]/25)*25)*2.4);
	IDUMMY=IHR*100+MIN;
	if (IWRD[2][9] != 9) {
		IPR[2] = 5510;
		IPR[3] = IDUMMY;
		goto MMSG;
	}
MMSF_5600:
	if (IWRD[2][9] == 21) {
		IPR[2] = 5610;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[2][9] == 6) {
		IPR[2] = 5710;
		IPR[3] = ITST[4]/1000;
		goto MMSG;
	}
	if ((IWRD[2][9] == 13) && (ITST[10] == 13)) {
		IPR[2] = 5720;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[2][9] == 13) {
		IPR[2] = 5730;
		IPR[3] = 0;
		goto MMSG;
	}		
	if ((IWRD[2][9] > 9) && (IWRD[2][9] < 13) && (IWRD[2][9] == ITST[10])){
		IPR[2] = 5740;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IWRD[2][9] == 22) && (IVEN[22] < 21000)) {
		IPR[2] = 5750;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IWRD[2][9] == 22) && (IVEN[22] > 21000)) {
		IPR[2] = 5755;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[2][9] == 20) {
		IPR[2] = 5780;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[2][9] == 18) {
		IPR[2] = 5781;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[2][9] == 35) {
		IPR[2] = 5782;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[2][9] != 64) goto MMSF_5820;
	IC=0;
	for (I=2; I<=14; I++) {
		if (I == 5) continue;
		if (IFSD(IRES[I]) == IR) IC=I;
	}
	IPR[2] = 5812;
	IPR[3] = IC;
	goto MMSG;
MMSF_5820:
	if (IWRD[2][9] == 17) {
		IPR[2] = 5830;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[2][9] == 67) {
		IPR[2] = 5860;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[2][9] == 29) {
		IPR[2] = 5870;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[2][9] == 90) {
		IPR[2] = 5880;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IWRD[2][9] == 88) && (IVEN[88] == 1017)) {
		IPR[2] = 2037;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[2][9] == 88) {
		IPR[2] = 5885;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IWRD[2][9] == 65) || (IWRD[2][9] == 66)){
		IPR[2] = 5886;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IWRD[2][9] == 79) && (IR == 51)) {
		IPR[2] = 5888;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IWRD[2][9] == 79) && (IR == 53)) {
		IPR[2] = 5890;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[2][9] == 49) {
		IPR[2] = 5891;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[2][9] == 48) {
		IPR[2] = 5892;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IWRD[2][9] == 97) || (IWRD[2][9] == 98)){
		IPR[2] = 5893;
		IPR[3] = 0;
		goto MMSG;
	}
	IPR[2] = 5910;
	IPR[3] = 0;
	goto MMSG;
MMSF_6000:
	if (IWRD[1][8] == 3) goto MMSF_6100;
	if (IWRD[0][10] == 1) {
		IPR[2] = 1000;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IWRD[1][8] != 4) && (IWRD[0][10] != 2)){
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IFSD(IRES[IWRD[1][9]]) == IR) {
		IPR[2] = 6005;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IVEN[30] < 20000) 	{
		IPR[2] = 6010;
		IPR[3] = 0;
		goto MMSG;
	}
	IC=IFSD(IRES[IWRD[1][9]]);
	if (IC == 0) {
		IPR[2] = 520;
		IPR[3] = 0;
		goto MMSG;
	}
	IC=IFSD(IROM[IC]);
	IPR[2] = 6020;
	IPR[3] = IC;
	goto MMSG;
MMSF_6100:
	if (IWRD[0][10] != 2) {	
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IVEN[IWRD[1][9]] > 20000) {
		IPR[2] = 2020;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IVEN[IWRD[1][9]] > 10000) && (IFSD(IVEN[IWRD[1][9]]) == IR)) {
		IPR[2] = 6005;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IVEN[49] < 20000) {
		IPR[2] = 6030;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IFSD(IVEN[IWRD[1][9]]) == 0) {
		IPR[2] = 6035;
		IPR[3] = 0;
		goto MMSG;
	}
	IPR[2] = 6040;
	IPR[3] = 0;
	goto MMSG;
MMSF_7000:
	if ((IWRD[0][10] != 1) && ((IWRD[0][10] != 2) || (IWRD[1][8] != 5) ||
	    (IWRD[1][9] != 11))) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSG;
	}
	printf("\nYOUR BOOTY CONTAINS:");
	MMRI(2, 0);
	IPR[2] = 0;
	IPR[3] = 0;
	goto MMSG;

MMSF_8000:
	if (IWRD[0][10] == 1) {
		IPR[2] = 1000;
		IPR[3] = 0;
		goto MMSG;
	}
	if((IWRD[0][9] == 30) && (((IWRD[1][8] == 5) && 
	   (IWRD[1][9] != 2)) || ((IWRD[2][8] == 5) && (IWRD[2][9] != 2)) ||
	   ((IWRD[1][8] != 5) && (IWRD[2][8] != 5)))) goto MMSF_11000;
	if(((IWRD[1][8] != 3) && (IWRD[2][8] != 3)) ||
	    (IWRD[0][10] > 3) || (IWRD[0][10] == 1) || 
	    ((IWRD[0][10] == 2) && (IWRD[0][9] == 30))) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[1][8] == 5) IWRD[1][9] = IWRD[2][9];
	if ((IWRD[1][9] < 54) && (IVEN[IWRD[1][9]] < 20000)) {
		IPR[2] = 3012;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IWRD[1][9] > 47) && 
	    ((IVEN[IWRD[1][9]] - (IVEN[IWRD[1][9]]/100)*100) != IR)) {
		IPR[2] = 8031;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IWRD[1][9] != 4) && (IWRD[1][9] != 19) && (IWRD[1][9] != 26) &&
	    (IWRD[1][9] != 27) && (IWRD[1][9] != 31) && (IWRD[1][9] != 75) &&
	    (IWRD[1][9] != 76) && (IWRD[1][9] != 89)) {
		IPR[2] = 8010;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IVEN[IWRD[1][9]]/1000 - (IVEN[IWRD[1][9]]/10000)*10) == 1) {
		IPR[2] = 8020;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[1][9] == 27) {
		IPR[2] = 8060;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IWRD[1][9] != 19) && (IWRD[1][9] != 76) && (IWRD[1][9] != 89)) 
		goto MMSF_8010;
	if ((IVEN[31] < 20000) && (IVEN[19] < 21000) && ((IR != 10) ||
	    (IVEN[76] < 1000)) && ((IR != 14) || (IVEN[89] < 1000))) {
		IPR[2] = 8050;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((((IWRD[1][9] == 76)||(IWRD[1][9] == 89)) && (IVEN[19] < 21000)) ||
	   ((IWRD[1][9] == 19) && ((IR != 10) || (IVEN[76] < 1000)) &&
	   ((IR != 14) || (IVEN[89] <  1000)))) IVEN[31]=21100;

MMSF_8010:
	if ((IWRD[1][9] == 4) && ((ITST[6]-(ITST[6]/1000)*1000) > 119) &&
	    (IVEN[8] < 20000)) {
		IPR[2] = 8035;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IWRD[1][9] == 4) && ((ITST[6]-(ITST[6]/1000)*1000) > 119))
		ITST[6] -= 2;
	IVEN[IWRD[1][9]] = IVEN[IWRD[1][9]] + 1000;
	if ((IWRD[1][9] == 75) && (ITST[8] != 0)) {
		IPR[2] = 8040;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IWRD[1][9] == 75) && (ITST[8] == 0)) {
		IPR[2] = 8045;
		IPR[3] = 0;
		goto MMSG;
	}
	if (ITFD(IROM[IR]) == 50) {
		IPR[2] = 1040;
		IPR[3] = 0;
		goto MMSG;
	}
	IWRD[0][10] = 1;
	IWRD[0][9]  = 25;
	IPR[2]=0;
	IPR[3]=0;
	goto MMSC;
MMSF_9000:
	if (IWRD[0][10] > 1)  {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSG;
	}
	R = RN(R);
	IC=(int)(R*4+9040);
	if ((IR == 93) && (ITST[14] == 93)) IC = 9044;
	if ((IR == 98) || (IR == 97)) IC = 9045;
	if (IR == 83) IC=9046;
	if (IR == 19) IC=9047;
	if ((IR == 40) && (IXT[141] == 0)) IC = 9048;
	if ((IR == 78) || (IR == 80)) IC = 9049;
	if (IR == 86) IC=9050;
	if (((IR < 35) && (IR > 27)) || ((IR < 52) && (IR > 41))) IC = 9051;
	if ((IR == 8) && (IXT[84] == 0)) IC = 9048;
	if (IC > 9043) 
		printf("\nHINTS COST 20 POINTS EACH. DO YOU WANT ONE?  ");
	if (IC < 9044) 
		printf("\nHINTS WASTE 20 POINTS EACH. DO YOU WANT ONE?  ");
MMSF_9020:
	IANS = getchar();
	if (IANS > 'a')
		IANS -= ' ';
	if (IANS == 'N') {
		IPR[2] = 1040;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IANS != 'Y') {
		printf("\nWHAT? PLEASE ANSWER YES OR NO");
		goto MMSF_9020;
	}
	if (ITST[2] < 20) {
		IPR[2] = 9027;
		IPR[3] = 0;
		goto MMSG;
	}
	ITST[2] -= 20;
	ITST[3] -= 20;
	IPR[2] = IC;
	IPR[3] = 0;
	goto MMSG;
MMSF_10000:
	if (IWRD[0][10] == 1) {
		IPR[2] = 1000;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IWRD[0][10] != 2) || (IWRD[1][8] != 3)) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IR == 19) && ((IWRD[0][9] == 33) && (IWRD[1][9] = 92))) {
		IXT[11]=2219;
		IPR[2] = 1095;
		IPR[3] = IR;
		goto MMSG;
	}
	if ((IR == 5) && (IWRD[0][9] == 32) && (IWRD[1][9] == 70)) {
		IXT[183]=405;
		IPR[2] = 1095;
		IPR[3] = IR;
		goto MMSG;
	}
	if ((IR == 7) && (IWRD[0][9] == 32) && (IWRD[1][9] == 69)) {
		IXT[123]=407;
		IPR[2] = 1095;
		IPR[3] = IR;
		goto MMSG;
	}
	if ((IWRD[1][9] == 88) && (IR == 17)) {
		IPR[2] = 2037;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[1][9] != 92) goto MMSF_10050;
	if (((IR > 27) && (IR != 91) && (IR != 54) && (IR != 36) &&
	    (IR != 90)) || (IR == 3) || (IR == 4) || (IR == 13) ||
	    (IR == 16) || (IR == 21) || (IR == 22)) {
		IPR[2] = 2030;
		IPR[3] = 0;
		goto MMSG;
	}
	for (J=1; J<=240; J++) {
		if (IFSD(IXT[J]) != IR) continue;
		if (((J-1)/60+1) != (ITST[4]/1000)) continue;
		IC = ITFD(IXT[J]);
		if ((IC == 4) || (IC == 13) || (IC == 22) || (J == 62) ||
		    (J == 70) || (J == 133) || (J == 182) || (J == 190)) {
			IPR[2] = 15030;
			IPR[3] = 0;
			goto MMSG;
		}
		if ((IXT[J] > 20000) && (IWRD[0][9] == 32)) {
			IPR[2] = 10035;
			IPR[3] = 0;
			goto MMSG;
		}
		if (IWRD[0][9] == 32) {
			IPR[2] = 10045;
			IPR[3] = 0;
			goto MMSG;
		}
		IPR[2] = 10046;
		IPR[3] = 0;
		goto MMSG;
	}
	IPR[2] = 15030;
	IPR[3] = 0;
	goto MMSG;
MMSF_10050:
	if (IWRD[1][9] != 91) goto MMSF_10100;
	if ((IR > 26) || (IR == 21) || (IROM[IR] < 10000)) {
		IPR[2] = 10010;
		IPR[3] = 0;
		goto MMSG;
	}
	if (((IROM[IR] > 20000) && (IWRD[0][9] == 32)) ||
	    ((IROM[IR] < 20000) && (IWRD[0][9] == 33))) {
		IPR[2] = 8020;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[0][9] == 32) IROM[IR] += 10000;	
	if (IWRD[0][9] == 33) IROM[IR] -= 10000;	
	if ((IWRD[0][9] != 32) || (IFSD(IRES[5]) != IR) ||
	   (ITST[1] > 300)) goto MMSF_10090;
	for (J=1; J<=360; J++) {
		if (IFSD(IXT[J]) != IR) continue;
		if (IXT[J] > 20000) continue;
		if ((ITFD(IXT[J]) == 16) || (ITFD(IXT[J]) > 27)) continue;
		if (IROM[ITFD(IXT[J])] > 20000) continue;
		IRES[5] = IRES[5] - IR + ITFD(IXT[J]);
		goto MMSF_10090;
	}
	IRES[5]=0;
	IVEN[1] = 100+IR;
	ITST[2] += 75;
	IPR[2] = 10085;
	IPR[3] = 0;
	goto MMSG;
MMSF_10090:
	if (ITFD(IROM[IR]) == 50) goto MMSF_10095;
	IWRD[0][10] = 1;
	IWRD[0][9] = 25;
	IPR[2]=0; IPR[3]=0;
	goto MMSC;
MMSF_10095:
	IPR[2]=1040; IPR[3]=0;
	goto MMSG;
MMSF_10100:
	if (IWRD[1][9] != 93) goto MMSF_10200;
	if ((IR != 93) && (IR != 92) && (IR != 95) && (IR != 54) &&
	    (IR != 55)) {
		IPR[2] = 2030;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IXT[27] == 0) && ((IR == 54) || (IR == 55))) {
		IPR[2] = 10105;
		IPR[3] = 0;
		goto MMSG;
	}
	if (((IR == 95) || (IR == 55)) && (IWRD[0][9] == 32)) {
		IPR[2] = 8020;
		IPR[3] = 0;
		goto MMSG;
	}
	if (((IR == 95) || (IR == 55)) && (IWRD[0][9] == 33)) {
		IPR[2] = 10110;
		IPR[3] = 0;
		goto MMSG;
	}
	if (((IWRD[0][9] == 33) && (IXT[54] > 20000)) || 
	    ((IWRD[0][9] == 32) && (IXT[54] < 20000))) {
		IPR[2] = 8020;
		IPR[3] = 0;
		goto MMSG;
	}
	for (J=1; J<=14; J++) {
		if (((IFSD(IRES[J]) != IR) || (IRES[J] > 10000)) &&
		    (IVEN[48] < 20000)) continue;
		if (IWRD[0][9] == 32) IXT[54]=19293;
		if (IWRD[0][9] == 32) IXT[234]=19392;
		if (IWRD[0][9] == 33) IXT[54]=29293;
		if (IWRD[0][9] == 33) IXT[234]=29392;
		if (J > 1) {
			IPR[2] = 10115;
			IPR[3] = J;
			goto MMSG;
		}
		IPR[2] = 10116;
		IPR[3] = J;
		goto MMSG;
	}
	IPR[2] = 10130;
	IPR[3] = 0;
	goto MMSG;
MMSF_10200:
	if ((IWRD[1][9] < 54) && (IVEN[IWRD[1][9]] < 20000)) {
		IPR[2] = 3010;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[1][9] != 35) goto MMSF_10300;
	if (((IWRD[0][9] == 32) && (IFD(IVEN[35]) == 2)) || 
	    ((IWRD[0][9] == 33) && (IFD(IVEN[35]) < 2))) {
		IPR[2] = 8020;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[0][9] == 33) {
		IVEN[35] -= 1000;
		IPR[2] = 1040;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IFD(IVEN[35]) == 0) {
		IPR[2] = 5783;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IFD(IVEN[35]) == 5) {
		IVEN[35] += 1000;
		IPR[2] = 1040;
		IPR[3] = 0;
		goto MMSG;
	}
	IVEN[35] += 1000;
	for (J=1; J<=360; J++) {
		if ((IFSD(IXT[J]) != IR) || (IXT[J] > 20000)) continue;
		IVEN[35] -= 100;
		IPR[2] = 10210;
		IPR[3] = 0;
		goto MMSG;
	}
	IPR[2] = 10230;
	IPR[3] = 0;
	goto MMSG;
MMSF_10300:
	IPR[2] = 10910;
	IPR[3] = 0;
	goto MMSG;
MMSF_11000:
	if (IWRD[0][10] == 1) {
		IPR[2] = 1000;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IWRD[0][9] == 30) && (((IWRD[1][8] == 5) && (IWRD[1][9] != 10)) ||
	   ((IWRD[2][8] == 5) && (IWRD[2][9] != 10)) ||
	   ((IWRD[1][8] != 5) && (IWRD[2][8] != 5)))) goto MMSF_12000;
	if ((IWRD[0][9] == 35) && (((IWRD[1][9] == 2) && (IWRD[1][8] == 5) && 
	    (IWRD[2][9] == 88) && (IWRD[2][8] == 3) && (IWRD[0][10] == 3)) ||
	   ((IWRD[0][10] == 2) && (IWRD[1][9] == 88) && (IWRD[1][8] == 3)))) {
		IPR[2] = 2038;
		IPR[3] = 0;
		goto MMSG;
	}
	if (((IWRD[0][10]  ==  2)  &&  (IWRD[0][9]  !=  34)) ||
	    ((IWRD[0][10]  ==  3)  &&  (IWRD[1][8]  !=  5)  && 
	     (IWRD[2][8]  !=  5))) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSG;
	}
	if (((IWRD[1][8] == 5)  &&  (IWRD[1][9]  !=  10)  && 
	    (IWRD[1][9]  !=  8)) || ((IWRD[2][8] == 5)  && 
	    (IWRD[2][9]  !=  10)  &&  (IWRD[2][9]  !=  12))) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[0][10] > 3) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IWRD[1][8] == 5) IWRD[1][9]=IWRD[2][9];
	if (IWRD[1][8] == 5) IWRD[1][0]=IWRD[2][0];
	if (IWRD[1][8] == 5) IWRD[1][1]=IWRD[2][1];
	if (IWRD[1][8] == 5) IWRD[1][2]=IWRD[2][2];
	if (IWRD[1][8] == 5) IWRD[1][3]=IWRD[2][3];
	if (IWRD[1][8] == 5) IWRD[1][8]=IWRD[2][8];
	IC=IWRD[1][9];
	if ((IC < 36) && (IVEN[IC] < 20000)) {
		IPR[2] = 3010;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IC > 35) && (IFSD(IVEN[IC]) != IR)) {
		IPR[2] = 8031;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IC != 4) && (IC != 19) && (IC != 26) && (IC != 27) &&
	    (IC != 31) && (IC != 44) && (IC != 45)) {
		IPR[2] = 8010;
		IPR[3] = 0;
		goto MMSG;
	}
	if (((IVEN[IC]/1000) - (IVEN[IC]/10000)*10) == 0) {
		IPR[2] = 8020;
		IPR[3] = 0;
		goto MMSG;
	}
	IVEN[IC] -= 1000;
	IPR[2] = 1040;
	IPR[3] = 0;
	goto MMSG;

MMSF_12000:
	if ((IR == 10) && ((IWRD[1][8] == 3) && (IWRD[1][9] == 72) &&
	    (IWRD[0][10] == 2))) {
		IXT[5] = 1310;
		IPR[2] = 1095;
		IPR[3] = IR;
		goto MMSG;
	}
	if ((IWRD[1][8] == 3) && (IWRD[1][9] == 17) && (IWRD[0][10] == 2))
		goto MMSF_12100;
	IC=IWRD[1][9];
	if ((IWRD[0][10] != 2) || (IWRD[1][8] != 2) || (IC == 5) ||
	    (IC == 6) || (IC > 10)) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSG;
	}
	if ((IC < 5) && (IVEN[6] < 20000) && (IR < 54)) {
		IPR[2] = 1005;
		IPR[3] = 0;
		goto MMSG;
	}
	if (IC < 5) {
		ITST[4] = ITST[4] - (ITST[4]/1000)*1000 + IC*1000;
		IPR[2] = 1040;
		IPR[3] = 0;
		goto MMSG;
	}
	ITST[4] = ITST[4]+1000*(IC-9);
	if (ITST[4] < 1000) ITST[4] += 4000;
	if (ITST[4] > 5000) ITST[4] -= 4000;
	IPR[2] = 1040;
	IPR[3] = 0;
	goto MMSG;
MMSF_12100:
	if ((IVEN[17] > 20000) || ((IFSD(IVEN[17]) == IR) && 
	    (IFD(IVEN[17]) != 1) && (IVEN[17] < 10000))) {
		IPR[2] = 1040;
		IPR[3] = 0;
		goto MMSG;
	}
	if (((IFSD(IVEN[17]) != IR) && (IVEN[17] < 10000)) ||
	     (IVEN[17] > 10000)) {
		IPR[2] = 8031;
		IPR[3] = 0;
		goto MMSG;
	}
	for (I=1; I<=360; I++) {
		if (((ITFD(IXT[I]) > 41) && (ITFD(IXT[I]) < 54)) ||
		    ((IFSD(IXT[I]) > 41) && (IFSD(IXT[I]) < 54))) IXT[I]=0;
	}
	IROM[42]=5153;
	for (I=43; I<=50; I++) IROM[I]=5053;
	IROM[51]=6555;
	IROM[52]=6554;
	IROM[53]=6555;
	for (I=1; I<=53; I++) {
		if ((IVEN[I] < 10000) && (IFSD(IVEN[I]) > 41) &&
		    (IFSD(IVEN[I]) < 54)) IVEN[I] = 0;
	}

	IVEN[40]=1151; IVEN[42]=1151; IVEN[37]=1153; IVEN[39]=1253;
	IRES[4]=10000;
	ITST[32]=ITST[5];
	IXT[22]=4243; IXT[23]=4244; IXT[82]=4442; IXT[83]=4445; IXT[84]=4544;
	IXT[85]=4750; IXT[86]=4948; IXT[87]=5049;
	IXT[142]=4344; IXT[143]=4443; IXT[202]=4342; IXT[203]=4345;
	IXT[204]=4543; IXT[205]=5047; IXT[254]=4142; IXT[255]=4547;
	IXT[256]=5148; IXT[257]=5249; IXT[258]=5350; IXT[315]=4241;
	IXT[316]=4648; IXT[317]=4647; IXT[318]=4745; IXT[351]=4849;
	IXT[352]=4851; IXT[353]=4950; IXT[319]=4952; IXT[354]=5053;
	IPR[2]=12140;
	IPR[3]=0;
	goto MMSG;
	IPR[2]=0;
	IPR[3]=0;
	goto MMSB;

MMSG:
	IRIT=IPR[2];
	IW63=IWRD[2][9];
	R=RND;
	IFC=ITST[4]/1000;
	IDN=1;
	IDE=2;
	IDS=3;
	IDW=4;
	if (IVEN[6] < 20000) IDN=10-IFC;
	if (IDN == 6) IDN=10;
	if (IVEN[6] < 20000) IDE=11-IFC;
	if (IVEN[6] < 20000) IDS=12-IFC;
	if (IDS == 11) IDS=7;
	if (IVEN[6] < 20000) IDW=13-IFC;
	if (IDW > 10) IDW=IDW-4;

	if (IRIT == 320) 
		printf("\nI CANNOT FIGURE OUT WHAT YOU ARE TRYING TO SAY");
	if (IRIT == 520) 
		printf("\nNOTHING HAPPENS");
	if (IRIT == 1000) {
		printf("\nI AM NOT A MINDREADER, YOU WILL HAVE TO TELL ME "
			" WHAT TO ");
		for (L=0; L<8; L++) 
			putchar(IWRD[0][L]);
	}
	if (IRIT == 1040) printf("\nOKAY");
	if (IRIT == 1065) printf("\nPROGRAM ERROR AT LINE %d", IPR[3]);
	if ((IPR[3] == 7) || (IPR[3] == 16) || (IPR[3] == 25)) IC = IDS;
	if ((IPR[3] == 5) || (IPR[3] == 14) || (IPR[3] == 23)) IC = IDW;
	if ((IPR[3] == 1) || (IPR[3] == 10) || (IPR[3] == 19)) IC = IDN;

	/*
	 * "open coffin" messages
	 */
	if (IRIT == 1095) {
		printf("\nYOUR ACTION OPENED A PANEL INTO THE SECRET PASSAGE "
			"THAT YOU CAN\nENTER BY GOING ");
		for (I=0; I<8; I++) 
			putchar(IDTN[IC][I]);
		putchar('\n');
		if (ITST[1] == 5) {
			/* If the vampire has never been seen, then put him */
			/* here.                                            */
			if (IRES[5] == 300)
				IRES[5] = 5;
			if (ITST[5] > 50)
				printf("\nTHE COFFIN IS EMPTY AND "
				       "THE LID WILL NOT STAY OPEN");
			else if (IRES[5] == 5)
				 printf("\nTHE VAMPIRE IS IN THE COFFIN "
				        "AND IS WAKING UP.");
		}
	}

	/*
	 * state of lighting messages
	 */
	if (IRIT == 1150) {
		printf("\nIT IS DARK HERE\n");
		if (IPR[3] == 1)
			printf("\nIT IS AFTER SUNSET");
		if (IPR[3] == 2)
			printf("\nA CURTAIN BLOCKS THE LIGHT");
	}

	if (IRIT == 2010) {
		printf("\nYOU CANNOT ");
		for (L=0; L<4; L++) 
			putchar(IWRD[0][L]);
		printf(" THE ");
		for (L=0; L<8; L++) 
			putchar(IWRD[1][L]);
	}
	if (IRIT == 2011) {
		printf("\nYOU CANNOT ");
		for (L=0; L<4; L++) 
			putchar(IWRD[0][L]);
		printf(" THE ");
		for (L=0; L<8; L++) 
			putchar(IWRD[1][L]);
	}
	if (IRIT == 2020) {
		printf("\nYOU ALREADY HAVE THE ");
		for (L=0; L<8; L++) 
			putchar(IWRD[1][L]);
	}
	if (IRIT == 2025) {
		printf("\nIT IS NOT IN THE POSSESSION OF THE ");
		for (L=0; L<8; L++) 
			putchar(IWRD[3][L]);
	}
	if (IRIT == 2030) {
		printf("\nI DON'T SEE THE ");
		for (L=0; L<8; L++) 
			putchar(IWRD[1][L]);
	}
	if (IRIT == 2035) {
		printf("\nTHE WOOD IS STACKED TOO HIGH.");
	}
	if (IRIT == 2037) 
		printf("\nTHE DELICATE PAPYRUS OF THE SCROLL DISINTEGRATED "
			"WHEN YOU TOUCHED IT.");
	if (IRIT == 2038) 
		printf("\nI KNEW YOU WOULD HAVE TO TRY THAT. WELL YOU JUST "
			"BLEW PIECES\nOF PAPYRUS ALL OVER THE LIBRARY.");
	if ((IRIT == 2037) || (IRIT == 2038)) IVEN[88]=1017;
	if (IRIT == 2040) {
		printf("\nYOU CANNOT CARRY THE ");
		for (L=0; L<8; L++) 
			putchar(IWRD[1][L]);
	}
	if (IRIT == 2041) 
		printf("\nTHE GOLD BULL FIGURINE AND BARS WERE DELICATELY "
			"BALANCED AND FELL.");
	if (IRIT == 2042) 
		printf("\nTHE DAGGER MADE A LOUD GRATING SOUND WHEN YOU PULLED "
			"IT OUT OF THE SLAB.");
	if ((IRIT == 2041) || (IRIT == 2042))  {
		printf(" THE NOISE WOKE THE TROLL AND HE BLUDGEONED YOU TO "
			"DEATH.");
		IPR[2] = 10030;
		IPR[3] = 0;
		goto MMSD;
	}
	if (IRIT == 2060) 
		printf("\nYOU WILL HAVE TO DROP SOMETHING FIRST");
	if (IRIT == 2070) {
		printf("\nYOUR BOOTY NOW CONTAINS THE ");
		for (L=0; L<8; L++) 
			putchar(IWRD[1][L]);
	}
	if (IRIT == 2110) 
		printf("\nI WOULD RECOMMEND THAT YOU TAKE YOUR SHOWERS IN "
			"THE BATHROOM.");
	if (IRIT == 2120) 
		printf("\nYOU BETTER DROP YOUR BOOTY FIRST.");
	if (IRIT == 2140) 
		printf("\nYOU WOULD JUST GET YOUR CLOTHES WET.");
	if (IRIT == 2150) 
		printf("\nYOU ARE NOW REFRESHED AND SMELL A LOT BETTER ALSO.");
	if (IRIT == 3010) {
		printf("\nYOU DO NOT HAVE THE ");
		for (L=0; L<8; L++) 
			putchar(IWRD[1][L]);
	}
	if (IRIT == 3012) 
		printf("\nYOU DO NOT HAVE IT.");
	if (IRIT == 3015) {
		printf("\nA WOOD NYMPH JUST RAN OUT FROM THE FOREST AND "
			"RAN BACK IN AFTER PICKING\nUP THE ");
		for (L=0; L<8; L++) 
			putchar(IWRD[1][L]);
	}
	if (IRIT == 3030) 
		printf("\nYOU HEAR A THUD AS IT HITS THE BOTTOM OF THE WELL");
	if (IRIT == 3040) 
		printf("\nYOU HEAR A SPLASH AS IT HITS THE WATER IN THE WELL");
	if (IRIT == 3045) 
		printf("\nTHE GLOBE JUST BROKE INTO A THOUSAND PIECES, "
			"MOMENTARILY RELEASING\nA CLOUD OF HAZY SMOKE THAT "
			"QUICKLY SURROUNDS YOU WITH VISIONS OF\nTHE "
			"INHABITANTS OF MYSTERY MANSION LAUGHING AT YOU "
			"HIDEOUSLY.");
	if (IRIT == 3110) {
		printf("\nYOU NO LONGER HAVE THE ");
		for (L=0; L<8; L++) 
			putchar(IWRD[1][L]);
	}
	if (IRIT == 3210) 
		printf("\nI WOULDN'T DO THAT. THE NYMPH WILL TAKE IT ALL.");
	if (IRIT == 4010) {
		printf("\nFIRST YOU HAVE TO FIND THE ");
		for (L=0; L<8; L++) 
			putchar(IWRD[3][L]);
	}
	if (IRIT == 4030) {
		printf("\nYOU CANNOT GIVE IT TO THE ");
		for (L=0; L<8; L++) 
			putchar(IWRD[3][L]);
	}
	if ((IRIT == 3110) && (IPR[3] == 1)) {
		printf("\nYOU GAVE IT TO THE ");
		for (L=0; L<8; L++) 
			putchar(IWRD[3][L]);
	}
	if (IRIT == 4050) 
		printf("\nTHANKS. IT NEEDS THAT AROUND HERE.");
	if (IRIT == 5010) 
		printf("\nYOU CAN ONLY LOOK AT ITEMS AND INHABITANTS");
	if (IRIT == 5020) {
		printf("\nI DO NOT SEE THE ");
		for (L=0; L<8; L++) 
			putchar(IWRD[2][L]);
	}
	if (IRIT != 5070) goto MMSG_5100;
	if ((IRIT == 5070) && (IRES[IW63] > 10000))
		printf("\nAND IS DEAD");
	if ((IRES[IW63]/1000) == 9)
		printf("\nIS SEVERELY WOUNDED AND WILL SOON BE DEAD.");
	if ((IRES[IW63]/1000) == 8)
		printf("\nIS BADLY WOUNDED AND MIGHT DIE.");
	if ((IRES[IW63]/1000) == 7)
		printf("\nIS WOUNDED AND HAS PASSED OUT.");
	if ((IRES[IW63]/1000) == 6)
		printf("\nAND HAS BEEN KNOCKED UNCONSCIOUS.");
	if ((IRES[IW63]/1000) == 5)
		printf("\nAND HAS BEEN SHOT IN THE LEG.");
	if ((IRES[IW63]/1000) == 4)
		printf("\nAND IS WOUNDED.");
	if ((IRES[IW63]/1000) == 3)
		printf("\nAND IS WOUNDED BUT MIGHT RECOVER.");
	if ((IRES[IW63]/1000) == 2)
		printf("\nIS WOUNDED AND WILL BE BETTER SOON.");
	if ((IRES[IW63]/1000) == 1)
		printf("\nIS HURT AND MOVING SLOWLY.");
	if ((IRIT == 5070) && (IRES[IW63] > 1000) && (IW63 != 1)) 
		goto MMSG_5100;
	if ((IRIT == 5070) && (IW63 == 2))
		printf("\n\nSHE IS AN OLDER THIN WOMAN WEARING A LONG BLACK "
			"DRESS.");
	if ((IRIT == 5070) && (IW63 == 3))
		printf("\n\nHE IS A HANDSOME YOUNG MAN AND "
			"IS SMARTLY DRESSED.");
	if ((IRIT == 5070) && (IW63 == 4))
		printf("\n\nIT IS A HUGE UGLY THING, THAT IS SLOW AND BLIND.");
	if ((IRIT == 5070) && (IW63 == 5))
		printf("\n\nHE IS PALE, HAS A LONG BLACK CAPE, "
			"AND IS BREATHING HEAVILY THROUGH\nHIS LONG CANINES.");
	if ((IRIT == 5070) && (IW63 == 6))
		printf("\n\nHE IS A FRIENDLY LOOKING HUSKY CHAP.");
	if ((IRIT == 5070) && (IW63 == 7))
		printf("\n\nHE IS A SMALL MAN WITH DIRTY CLOTHES AND HANDS.");
	if ((IRIT == 5070) && (IW63 == 8))
		printf("\n\nSHE IS A SEXY YOUNG WOMAN WITH LONG BLOND HAIR.");
	if ((IRIT == 5070) && (IW63 == 9))
		printf("\n\nSHE IS DRESSED IN WHITE AND "
			"HAS BLOOD ON HER APRON.");
	if ((IRIT == 5070) && (IW63 == 10))
		printf("\n\nHE IS A HAIRY BEAST AND HIS EYES ARE LOOKING "
			"DIRECTLY AT YOU.");
	if ((IRIT == 5070) && (IW63 == 11))
		printf("\n\nHE IS A LITTLE FELLOW WITH FUNNY CLOTHES AND "
			"SNEAKY EYES.");
	if ((IRIT == 5070) && (IW63 == 12))
		printf("\n\nHE IS A HEALTHY LOOKING MAN WITH A FULL BEARD.");
	if ((IRIT == 5070) && (IW63 == 13))
		printf("\n\nHE IS A MEAN LOOKING LITTLE THING WITH GLOWING "
			"EYES.");
	if ((IRIT == 5070) && (IW63 == 14))
		printf("\n\nHE IS AN OLDER GENTLEMAN WITH GRAY HAIR AND "
			"THICK GLASSES.");
	if ((IRIT == 5070) && (IW63 == 15))
		printf("\n\nIT IS A LARGE HUNGRY LOOKING ANIMAL.");
	if ((IRIT == 5070) && (IW63 == 16))
		printf("\n\nHE IS A LARGE MUSCULAR MAN WITH HATE IN HIS EYES.");
	if ((IRIT == 5070) && (IW63 == 1))
		printf("\n\nFROM THE LOOK OF THE BODY IT HAS NOT BEEN DEAD "
			"VERY LONG.");
	if ((IRIT == 5070) && (IW63 == 1) && (ITST[10] < 13))
		printf("\n\nTHE VICTIM'S ARMS ARE OUTSTRETCHED AS IF HE WAS "
			"DRAGGED HERE.\nHIS SHIRT IS TORN AND ALL BLOODY.");
	if ((IRIT == 5070) && (IW63 == 1) && (ITST[10] == 13))
		printf("\n\nIT IS ALL DOUBLED OVER WITH HIS ARMS COVERING HIS "
			"STOMACH.");
	if ((IRIT == 5070) && (IW63 == 1) && (ITST[10] == 14))
		printf("\n\nIT IS ALL BEATEN AND HIS HEAD IS BASHED IN.");
	if ((IRIT == 5070) && (IW63 == ITST[9]))
		printf("\n\nYOU NOTICE A LOOK OF APPREHENSION.");
MMSG_5100:
	if (IRIT == 5110) {
		printf("\nYOU DO NOT HAVE THE ");
		for (L=0; L<8; L++) 
			putchar(IWRD[2][L]);
	}
	if (IRIT == 5130) 
		printf("\nON CLOSER INSPECTION IT APPEARS AS THOUGH THE "
			"GLOBE IS REALLY A DUSTY\nCRYSTAL BALL WITH MAGICAL "
			"POWERS TO HELP YOU FIND INHABITANTS");
	if (IRIT == 5140) 
		printf("\nTHE ROPE IS ABOUT 50 FEET LONG WITH A GRAPLING "
			"HOOK ON ONE END");
	if (IRIT == 5170)
		printf("\nTHE MAP IS ONLY OF THE WOODS AND THE ROAD WHICH YOU "
			"ARE NOT NEAR.");
	if (IRIT == 5210)
		printf("\nTHE CANDLE IS ABOUT %d INCHES LONG", IW63);
	if ((IRIT == 5310) && (IVEN[27] > 21000)) 
		printf("\nTHE TORCH IS LIT");
	if ((IRIT == 5310) && (IVEN[27] < 21000)) 
		printf("\nTHE TORCH IS ALL BURNT OUT");
	if (IRIT == 5410)
		printf("\nYOUR GUN HAS %d SHOTS LEFT", ITST[11]);
	if (IRIT == 5510) 
		printf("\nTHE TIME IS %d:%d", 
			IPR[3]/100, IPR[3]-((IPR[3]/100)*100));
	if (IRIT == 5610) {
		printf("\nTHE BOOK CONTAINS WORDS I KNOW LIKE THE VERB ");
		for (L=0; L<8; L++) 
			putchar(IVRB[ITST[16]][L]);
	}
	if (IRIT == 5710) {
		printf("\nYOU ARE FACING ");
		for (L=0; L<8; L++) 
			putchar(IDTN[IPR[3]][L]);
	}
	if (IRIT == 5720) 
		printf("\nTHE VIAL IS EMPTY BUT SMELLS LIKE POISON");
	if (IRIT == 5730) 
		printf("\nTHE VIAL IS FULL OF POISON");
	if (IRIT == 5740) 
		printf("\nTHERE IS BLOOD ON IT");
	if (IRIT == 5750) {
		IVEN[22] += 1000;
		printf("\nTHE NOTE SAYS: ");
		if (ITST[5] <= 50) printf(" I CAN HELP YOU OPEN THE GATE");
		if ((ITST[5] > 50) && (ITST[5] <= 100))
			printf(" YOU CAN GO IN ANY DIRECTION BY JUST ENTERING "
				"THE FIRST LETTER");
		if ((ITST[5] > 100) && (ITST[5] <= 150) && (ITST[8] != 0))
			printf(" IF YOU HAVE THE MURDER WEAPON AND CAN GET "
				"THE MURDERER TO RETURN TO\nTHE SCENE OF THE "
				"CRIME WITH YOU, YOU WILL SCORE POINTS");
		if ((ITST[5] > 100) && (ITST[5] <= 150) && (ITST[8] == 0))
			printf(" CONGRATULATIONS FOR SOLVING THE MURDER SO "
				"QUICKLY.");
		if ((ITST[5] > 150) && (ITST[5] <= 200) && (IRES[5] != 0))
			printf(" WATCH OUT FOR THE VAMPIRE. IF YOU CAN "
				"DESTROY HIM, YOU WILL\nSCORE POINTS");
		if ((ITST[5] > 150) && (ITST[5] <= 200) && (IRES[5] == 0)) {
			printf("\nLAST MONTH, IN THIS MYSTERY, A PLAYER LEFT "
				"A CURTAIN OPEN AND\nTHE ");
			for (L=0; L<8; L++) 
				putchar(IRSN[ITST[39]][L]);
			printf("\nSAW THE FULL MOON THROUGH IT AND "
				"TRANSFORMED INTO A WEREWOLF.");
		}
		if ((ITST[5] > 200) && (ITST[5] <= 250)) 
			printf("\nYOU WILL HAVE TO GO DOWN INTO THE DARK "
				"PIT TO FIND A BATTERY\nFOR THE LANTERN");
		if ((ITST[5] > 250) && (ITST[5] <= 300)) 
			printf("\nBEWARE: THE MOON IS FULL TONIGHT AND SOMEONE "
				"HERE IS A WEREWOLF.\nKILL IT AND YOU WILL "
				"SCORE POINTS.");
	}
	RND1=RND*7.7-(int)(RND*7.7);
	IC=(int)(RND1*3);
	if (IRIT == 5750) {
		if ((IC == 0) && (ITST[5] > 300) && (ITST[5] <= 350) && 
		    (ITST[8] != 0)) {
			printf("\nIN CASE YOU HAVEN'T GUESSED, THE SCENE "
				"OF THE CRIME IS THE ");
			for (L=0; L<16; L++) 
				putchar(IRNM[IFSD(IROM[ITST[8]])][L]);
		}
		if ((IC == 1) && (ITST[5] > 300) && (ITST[5] <= 350) &&
		    (ITST[8] != 0)) {
			printf("\nIN CASE YOU HAVEN'T GUESSED, THE MURDER WAS "
				"COMMITTED WITH THE %s", IWP);
		}
		if ((IC == 2) && (ITST[5] > 300) && (ITST[5] <= 350) &&
		    (ITST[8] != 0)) {
			printf(" IN CASE YOU HAVEN'T GUESSED, THE MURDERER "
				"IS THE ");
			for (L=0; L<8; L++) 
				putchar(IRSN[ITST[9]][L]);
		}
		if ((ITST[5] > 300) && (ITST[5] <= 350) && (ITST[8] == 0))
			printf("\nMAGIC WORDS DO NOT WORK AFTER THE "
				"GARDEN CLOSES.");
		if ((ITST[5] > 350) && (ITST[5] <= 400)) 
			printf("\nYOU MIGHT BE ABLE TO SCORE POINTS WHEN YOU "
				"HEAR SOMETHING");
		if ((ITST[5] > 400) && (ITST[5] <= 450)) 
			printf("\nSOMEONE HAS TAKEN A LOT OF DYNAMITE INTO "
				"THE PASSAGES BELOW THE MANSION");
		if (ITST[5] > 450) 
			printf(" I'D GET OUT OF HERE IF I WERE YOU");
		printf("\nSIGNED: A FRIEND");
	}
	if (IRIT == 5775)
		printf("\nTHE NOTE SAYS THE SAME AS IT DID BEFORE");
	if (IRIT == 5780)
		printf("\nTHE BULLET IS OLD AND TARNISHED");
	if (IRIT == 5781)
		printf("\nTHE TALISMAN SAYS: HAVING THIS CAN HELP YOU GET "
			"REINCARNATED IF YOU DIE");
	if (IRIT == 5782)
		printf("\nTHE CHEST IS VERY OLD WITH MYSTERIOUS WRITING "
			"ON IT;");
	if (((IRIT == 5782) || (IRIT == 5783)) && (IFD(IVEN[35]) == 0)) 
		printf("\nTHERE IS A PADLOCK LOCKING AN UNUSUAL HASP OF "
			"KNOTTED WIRE.");
	if ((IRIT == 5782) && (IFSD(IRES[8]) == ITST[1]) && (IRES[8] < 10000))
		printf("\nTHIS CHEST CONTAINS A CLUE, READS THE MAID ");
	if ((IRIT == 5782) && (IFSD(IRES[14]) == ITST[1]) && 
	    (IRES[14] < 10000))
		printf("\nDON'T OPEN THIS CHEST IN AN UNLOCKED ROOM, READS "
			"THE MASTER BEHIND YOU");
	if ((IRIT == 5782) && (IFSD(IRES[2]) == ITST[1]) && 
	    (IRES[2] < 10000))
		printf("\nBE SURE TO CLOSE THE CHEST AS QUICKLY AS POSSIBLE, "
			"READS THE\nLADY IN FRONT OF YOU");
	if (IRIT == 5810) {
		printf("\nYOU CAN SEE YOUR REFLECTION AND THINGS IN THE ROOM "
			"BEHIND YOU");
		if (IPR[3] > 0) {
			for (L=0; L<8; L++) 
				putchar(IRSN[IPR[3]][L]);
			printf("\nALSO, BESIDE YOU IN THE MIRROR IS THE "
				"REFLECTION OF THE ");
		}
		if (IPR[3] == 2)
			printf("\nSHE IS AN OLDER THIN WOMAN WEARING A LONG "
				"BLACK DRESS.");
		if (IPR[3] == 3)
			printf("\nHE IS A HANDSOME YOUNG MAN AND IS SMARTLY "
				"DRESSED.");
		if (IPR[3] == 8)
			printf("\nSHE IS A SEXY YOUNG WOMAN WITH LONG BLOND "
				"HAIR.");
		if (IPR[3] == 10)
			printf("\nHE IS A HAIRY BEAST AND HIS EYES ARE LOOKING "
				"DIRECTLY AT YOU.");
	}
	if (IRIT == 5830)
		printf("\nTHE AMULET IS A LARGE ODD SHAPED JEWEL WITH EIGHT "
			"FACETS, FOUR BEING HEXAGONS.");
	if (IRIT == 5860)
		printf("\nTHE WOOD IS A STACK OF SEVERAL LARGE LOGS AND A "
			"COUPLE OF SMALL\nLOGS WEDGED AGAINST THE WALL BY "
			"THE FIREPLACE.");
	if ((IRIT == 5860) && (ITST[5] > 50))
		printf("THE LOGS ON THE TOP\nLOOK LIKE THEY WERE JUST PUT "
			"THERE.");
	if (IRIT == 5870)
		printf("\nTHE WEDGE IS A POINTED PIECE OF WOOD ABOUT A "
			"FOOT LONG AND IS USED TO\nKEEP THE LOGS FROM "
			"ROLLING.");
	if (IRIT == 5880)
		printf("\nTHE PLAQUE IS INSCRIBED:\nDO NOT STAY HERE LONG.  "
			"THE BOARDS ARE LOOSE AND YOU MIGHT FALL THROUGH.");
	if (IRIT == 5885)
		printf("\nTHE SCROLL IS ROLLED UP AND YOU CANNOT SEE THE "
			"WRITING.");
	if (IRIT == 5886)
		printf("\nYOU ARE LOOKING AT A DRAWING OF SOME KIND OF A "
			"LEVER SYSTEM CENTERED AROUND\nA TRIANGULAR CAVITY "
			"CUT IN THE WALL.");
	if ((IRIT == 5888) || (IRIT == 5890))
		printf("\nTHE TROLL IS A HUGE UGLY THING SLEEPING FITFULLY "
			"ON A PILE OF BONES.");
	if ((IRIT == 5888) && (IVEN[36] == 1151)) 
		printf(" IT HAS A BIG BLUDGEON IN ONE HAND AND A STRING OF "
			"PEARLS IN THE OTHER.");
	if ((IRIT == 5890) && (IVEN[39] == 1253)) 
		printf(" IT HAS A LARGE MACE IN ONE HAND AND A SILVER GOBLET "
			"IN THE OTHER.");
	if (IRIT == 5891)
		printf("\nIT IS A COLORFUL BIRD WITH A BAND ON ITS LEG THAT "
			"SAYS 'I CAN FIND THINGS.'");
	if (IRIT == 5892)
		printf("\nTHE GAUNTLET IS A SOFT METAL GLOVE THAT SEEMS TO "
			"GIVE YOU EXTRA STRENGTH.");
	if (IRIT == 5893)
		printf("\nTHE TABLES ARE IN 8 ROWS AND SOME OF THE TABLES "
			"HAVE FIGURES CARVED IN THEM.\nTHE TWO NEAREST YOU "
			"ARE MADE OF DIFFERENT KINDS OF WOOD AND HAVE "
			"THE FIGURES\nOF A KING AND QUEEN ON THEM.");
	if (IRIT == 5910)
		printf("\nI DON'T KNOW WHAT YOU EXPECT TO SEE");
	if (IRIT == 6005) {
		printf("\nIF YOU LOOK YOU CAN SEE THE ");
		for (L=0; L<8; L++) 
			putchar(IWRD[1][L]);
	}
	if (IRIT == 6010) 
		printf("\nYOU WOULD NEED A CRYSTAL BALL TO SEE ANYONE "
			"NOT WITH YOU");
	IC=1;
	if ((IFSD(IRES[IWRD[1][9]]) == 54) || 
	    (IFSD(IRES[IWRD[1][9]]) == 71) || 
	    (IFSD(IRES[IWRD[1][9]]) == 72) || 
	    (IFSD(IRES[IWRD[1][9]]) == 81) || 
	    (IFSD(IRES[IWRD[1][9]]) == 82) || 
	    (IFSD(IRES[IWRD[1][9]]) == 85) || 
	    (IFSD(IRES[IWRD[1][9]]) == 86) || 
	    (IFSD(IRES[IWRD[1][9]]) == 92) || 
	    (IFSD(IRES[IWRD[1][9]]) == 91))
		IC=2;
	if (((IFSD(IRES[IWRD[1][9]]) >= 73) && 
	    (IFSD(IRES[IWRD[1][9]]) <= 77)) || 
	    (IFSD(IRES[IWRD[1][9]]) == 99)) 
		IC=3;
	if ((IFSD(IRES[IWRD[1][9]]) ==  55)  ||  
	    (IFSD(IRES[IWRD[1][9]]) ==  57)  ||  
	    (IFSD(IRES[IWRD[1][9]]) ==  93)  ||  
	    (ITST[1] ==  95)) 
		IC=4;
	if (IRIT == 6020) {
		printf("\nAS YOU GAZE INTO THE CRYSTAL BALL, THE FIGURE OF "
			"THE ");
		for (L=0; L<8; L++) 
			putchar(IWRD[1][L]);
		printf("\nCOMES INTO FOCUS ");
		putchar(IPRP[IC][0]);
		putchar(IPRP[IC][1]);
		printf(" THE ");
		for (L=0; L<16; L++) 
			putchar(IRNM[IPR[3]][L]);
	}
	if (IRIT == 6030)
		printf("\nYOU DO NOT YET HAVE THE MEANS TO FIND ITEMS NOT "
			"NEAR YOU.");
	if ((IRIT == 6040) || (IRIT == 6035)) 
		printf("\nTHE PARROT FLEW AWAY FOR A FEW MINUTES AND CAME "
			"BACK SAYING:");
	if ((IRIT == 6040) && (IVEN[IWRD[1][9]] < 10000))  {
		printf("\nBAAAAAAKKK: "); 
		for (L=0; L<16; L++) 
			putchar(IRNM[IFSD(IROM[IFSD(IVEN[IWRD[1][9]])])][L]);
	}
	if (IRIT == 6035)
		printf("\nBAAAAAAKKK: SORRY CHARLIE!");
	if ((IRIT == 6040) && (IVEN[IWRD[1][9]] > 10000))  {
		printf("\nBAAAAAAKKK: "); 
		for (L=0; L<8; L++) 
			putchar(IRSN[IFSD(IVEN[IWRD[1][9]])][L]);
	}
	if (IRIT == 7050) 
		printf(" NOTHING");
	if (IRIT == 8010) 
		printf("\nI DON'T KNOW WHAT YOU EXPECT TO HAPPEN");
	if (IRIT == 8015) {
		printf("\nYOUR FOOD HAS BEEN EATEN BY THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[1][L]);
	}
	if (IRIT == 8020)
		printf("\nIT ALREADY IS");
	if (IRIT == 8030) {
		printf("\nI DON'T SEE THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[2][L]);
	}
	if (IRIT == 8031) {
		printf("\nI DON'T SEE THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[1][L]);
	}
	if (IRIT == 8035) 
		printf("\nTHE BATTERY IN YOUR LANTERN IS DEAD");
	if (IRIT == 8040) {
		printf("\nAFTER THE RADIO WARMS UP AND THE STATIC DIES "
			"DOWN YOU HEAR\nA NEWS REPORT THAT THE POLICE "
			"ARE LOOKING FOR THE ");
		for (L=0; L<8; L++)
			putchar(IRSN[ITST[9]][L]);
		printf("\nOF MYSTERY MANSION FOR SEVERAL MURDERS");
	}
	if (IRIT == 8045) 
		printf("\nTHE RADIO PLAYS SOFT MUSIC AFTER IT WARMS UP.");
	if (IRIT == 8050) 
		printf("\nYOU NEED A MATCH OR SOMETHING.");
	if (IRIT == 8060) 
		printf("\nTHE TORCH IS ALL BURNED OUT AND YOU CANNOT LIGHT IT");
	if (IRIT == 9027) 
		printf("\nYOU DON'T HAVE ENOUGH POINTS");
	if (IRIT == 9040)
		printf("\nDON'T ASK FOR HELP ANYMORE");
	if (IRIT == 9041)
		printf("\nQUIT WHILE YOU ARE STILL ALIVE");
	if (IRIT == 9042)
		printf("\nYOU WILL NEVER FIGURE IT ALL OUT");
	if (IRIT == 9043)
		printf("\nREAD WHAT I SAY CAREFULL.");
	if (IRIT == 9044) 
		printf("\nA GOOD WAY TO START IS BY SAYING 'GO WEST' OR 'GO "
			"EAST'");
	if (IRIT == 9045) 
		printf("\nKEEP MOVING AND YOU WILL GET OUT OF THE WOODS "
			"SOONER OR LATER");
	if (IRIT == 9046) 
		printf("\nTRY SAYING A MAGIC WORD");
	if (IRIT == 9047) 
		printf("\nTRY BRINGING A CHAIR IN HERE OR CLOSING THE DOOR");
	if (IRIT == 9048) 
		printf("\nUNLESS YOU KNOW A MAGIC WORD, THERE IS NOTHING "
			"I CAN DO FOR YOU");
	if (IRIT == 9049) 
		printf("\nTHERE IS NOTHING YOU CAN DO UNLESS YOU KNOW A "
			"MAGIC WORD OR HAVE A SHOVEL.");
	if (IRIT == 9050) 
		printf("\nUNLESS YOU KNOW A MAGIC WORD OR CAN OIL THE HINGE "
			"ON THE FIRE ESCAPE,\nTHERE IS NOTHING I CAN DO FOR "
			"YOU.");
	if (IRIT == 9051) 
		printf("\nTRY DROPPING THINGS TO USE AS A POINT OF REFERENCE.");
	if (IRIT == 10020) 
		printf("\nYOU HAVE NOT FIGURED OUT HOW TO DO THAT YET.");
	if (IRIT == 10035) 
		printf("\nTHE DOOR IS LOCKED");
	if (IRIT == 10045) 
		printf("\nTHE DOOR IS OPEN");
	if (IRIT == 10046) 
		printf("\nTHE DOOR IS CLOSED");
	if (IRIT == 10010) 
		printf("\nTHERE IS NO CURTAIN HERE");
	if (IRIT == 10085) 
		printf("\nYOU TRAPPED THE VAMPIRE IN THE DAYLIGHT AND HE "
			"SHRIVELED INTO NOTHING.");
	if (IRIT == 10105) 
		printf("\nTHE GARDEN GATE IS NOW CLOSED PERMANENTLY.");
	if (IRIT == 10110) 
		printf("\nTHE GATE IS STUCK OPEN");
	if (IRIT == 10115) {
		printf("\nWITH A MIGHTY SHOVE, YOU MOVE THE GATE WITH THE "
			"HELP OF THE ");
		for (L=0; L<8; L++)
			putchar(IRSN[IPR[3]][L]);
	}
	if (IRIT == 10116) 
		printf("\nTHE GAUNTLET ALLOWS YOU TO EASILY MOVE THE GATE.");
	if (IRIT == 10130) 
		printf("\nTHE GATE IS TOO HEAVY FOR YOU TO OPEN ALONE.");
	if (IRIT == 10210) 
		printf("\n1000 SCREAMING DEMONS FLEE THE CONFINES OF THE "
			"CHEST AFTER CENTURIES OF\nIMPRISONMENT AND SCATTER "
			"ABOUT THE MANSION AND GROUNDS TO TORMENT YOU AND\n"
			"IMPEDE YOUR EVERY MOVE");
	if (IRIT == 10230) {
		printf("\n1000 SCREAMING DEMONS FLEE THE CONFINES OF THE "
			"AND SCATTER ABOUT\nTHE ROOM LOOKING FOR A WAY OUT. "
			"FINDING NO WAY OUT THEY RETURN TO THE CHEST\n"
			"MURMURING SOMETHING ABOUT MURDER AND THE ");
		for (L=0; L<8; L++)
			putchar(IRNM[IFSD(IROM[ITST[8]])][L]);
	}
	if (IRIT == 10910) {
		printf("\nYOU CANNOT OPEN THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[1][L]);
	}
	if (IRIT == 12140)
		printf("\nTHE FLOOR SLID HALFWAY INTO THE WALL. AS YOU "
			"LOOK DOWN, YOU CAN\nSEE A DEEP SHAFT WITH A "
			"SPIRAL WALKWAY CARVED INTO THE RIM\nDESCENDING "
			"INTO THE VAST DARKNESS.");
	if (IRIT == 15020)
		printf("\nTHE DOOR YOU ARE FACING HAS NO LOCK");
	if (IRIT == 15030)
		printf("\nYOU ARE NOT FACING A DOOR");
	if (IRIT == 15040)
		printf("\nYOU NEED A KEY");
	if (IRIT == 15110)
		printf("\nYOUR KEY WILL NOT FIT THE PADLOCK");
	if (IRIT == 16010)
		printf("\nYOU NEED A GUN TO SHOOT ANYONE");
	if (IRIT == 16020)
		printf("\nYOUR GUN IS OUT OF BULLETS");
	if (IRIT == 16030)
		printf("\nYOU MISSED");
	if (IRIT == 16035) {
		printf("\nYOU JUST SMASHED THE PADLOCK TO PIECES");
		IVEN[35] += 1000;
	}
	if (IRIT == 16040) {
		printf("\nYOU JUST WASTED A SHOT. IT DID YOU NO GOOD TO SHOOT "
			"THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[1][L]);
	}
	if (IRIT == 16045) {
		printf("\nYOU KILLED THE DWARF AND HE DISAPPEARED IN A "
			"CLOUD OF GREASY BLACK SMOKE");
		IRES[13] += 100;
	}
	if (IRIT == 16050) {
		printf("\nYOU JUST SHOT AND KILLED THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[1][L]);
	}
	if (IRIT == 20030) 
		printf("\nDON'T BE SILLY");
	IPR[2]=0;
	IPR[3]=0;
	goto MMSB;
	
MMSH:
	IR=ITST[1];
	IW71=IWRD[0][10];
	IW52=IWRD[1][8];
	IW62=IWRD[1][9];
	R=RND;
	switch (IWRD[0][9] - 35) {
		case 1: goto MMSH_1000;
		case 2: goto MMSH_2000;
		case 3: goto MMSH_3000;
		case 4: goto MMSH_4000;
		case 5: goto MMSH_5000;
		case 6: goto MMSH_6000;
		case 7: goto MMSH_7000;
		case 8: goto MMSH_8000;
		case 9: goto MMSH_9000;
		case 10: goto MMSH_10000;
		case 11: goto MMSH_10000;
		case 12: goto MMSH_11000;
		case 13: goto MMSH_12000;
		case 14: goto MMSH_13000;
		case 15: goto MMSH_14000;
		case 16: goto MMSH_15000;
		case 17: goto MMSH_16000;
		case 18: goto MMSH_17000;
		default: 
			printf("\nMMSH error: %d\n", IWRD[0][9]-35);
			pak();
			exit(-1);
	}
MMSH_1000:
	if ((IW71 != 1) && ((IW52 != 3) || (IW62 > 35))) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IW71 == 1) IW62=24;
	if (IVEN[IW62] < 20000) {
		IPR[2] = 3010;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IW62 != 22) && (IW62 != 24) && (IW62 != 28) &&
	    (IW62 != 29) && (IW62 != 31)) {
		IPR[2] = 2010;
		IPR[3] = 0;
		goto MMSI;
	}
	IVEN[IW62]=0;
	if (IW62 == 24) {
		IPR[2] = 1010;
		IPR[3] = 0;
		goto MMSI;
	}
	IPR[2] = 1020;
	IPR[3] = 0;
	goto MMSI;
MMSH_2000:
	if (((IW52 != 3) && (IW52 != 4)) || ((IW52 == 4) && (IW62 != 8)) ||
	    ((IW52 == 3) && (IVEN[IW62] == 0))) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSI;
	}
	MMRLret=1;
	goto MMRL;
RLret1:
	if ((IWRD[1][9] == 88) && (IR == 17)) {
		IPR[2] = 2037;
		IPR[3] = 0;
		goto MMSI;
	}
	if (((IW52 == 4) && (IFSD(IRES[8]) != IR)) || 
	    ((IW52 == 3) && (IW62 > 35) && (IVEN[IW62] != IR))) {
		IPR[2] = 2030;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IW52 == 4) && (IR != 25)) {
		IPR[2] = 2080;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IW52 == 4) IXT[134] = 2225;
	if (IW52 == 4) {
		IPR[2] = 1095;
		IPR[3] = IR;
		goto MMSI;
	}
	if ((IW62 < 36) && (IVEN[IW62] < 20000)) {
		IPR[2] = 3010;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IW62 != 45) {
		IPR[2] = 520;
		IPR[3] = 0;
		goto MMSI;
	}
	IPR[2] = 2090;
	IPR[3] = 0;
	goto MMSI;
MMSH_3000:
	if ((IW71 != 2) || (IW52 != 2) || ((IW62 != 5) && (IW62 != 6))) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IW62 == 5) && (IVEN[23] < 20000)) {
		IPR[2] = 3050;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IW62 == 5) {
		ITST[1] = 89;
		IPR[2] = 3060;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IR != 89) {
		IPR[2] = 3070;
		IPR[3] = 0;
		goto MMSI;
	}
	IC = IFSD(IVEN[26]);
	if ((IVEN[26] == 789) || (IVEN[26] > 20000) || ((IVEN[26] < 20000) &&
	    (IVEN[26] > 10000) && (IFSD(IRES[IC]) == 89))) {
		IPR[2] = 3080;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IVEN[26] > 10000) IC=IFSD(IRES[IC]);
	if (IC == 0) {
		IPR[2] = 520;
		IPR[3] = 0;
		goto MMSI;
	}
	ITST[1] = IC;
	IPR[2] = 3090;
	IPR[3] = IC;
	goto MMSI;
MMSH_4000:
	if (IW71 != 1) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IVEN[25] < 20000) {
		IPR[2] = 4055;
		IPR[3] = 0;
		goto MMSI;
	}
	MMRLret=2;
	goto MMRL;
RLret2:
	if (IR != 23) {
		IPR[2] = 4050;
		IPR[3] = 0;
		goto MMSI;
	}
	IXT[191] = 2223;
	IPR[2] = 1095;
	IPR[3] = IR;
	goto MMSI;
MMSH_5000:
	if ((IW52 != 3) || (IW62 < 55) || (IW62 > 58)) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IR != 11) {
		IPR[2] = 8032;
		IPR[3] = 0;
		goto MMSI;
	}	
	IC=0;
	if ((IW62 == 55) && (ITST[8] == 0) && (IXT[54] < 20000) &&
	    (IFSD(IRES[ITST[9]]) == IR)) {
		IC=1;
		IRES[ITST[9]] = 0;
		ITST[2] = ITST[2]+50;
		IPR[2] = 5001;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IW62 == 58) && (IXT[52] != 0) && (ITST[5] > 400)) 
		IXT[52]=0;
	if ((IXT[52] == 0) && (IW62 == 58)) {
		IPR[2] = 5002;
		IPR[3] = 0;
		goto MMSI;
	}
	IPR[2] = 5009;
	IPR[3] = 0;
	goto MMSI;
MMSH_6000:
	if ((IW71 != 2) || (IW52 != 3) || (IVEN[IW62] == 0)) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSI;
	}
	MMRLret=6;
	goto MMRL;
RLret6:
	if (((IW62 == 94) || (IW62 == 97) || (IW62 == 98)) && (IR == 15)) {
		IPR[2] = 6005;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IW62 == 95) && (IR == 15) && (IVEN[48] == 0)) {
		IPR[2] = 6015;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IWRD[1][9] == 88) && (IR == 17)) {
		IPR[2] = 2037;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IW62 == 68) && (IR == 27)) {
		IPR[2] = 6030;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IW62 == 68) && (IR == 88)) {
		IPR[2] = 6040;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IW62 == 84) && ((IR < 42) || (IR > 50))) {
		IPR[2] = 8031;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IW62 == 84) && (IXT[320] != 0)) {
		IPR[2] = 1040;
		IPR[3] = 0;
		goto MMSI;
	}
	R = RN(R);
	if ((IW62 == 84) && (R < 0.333) && (IR > 42) && (IR < 47))  {
		IVEN[38] = 100+IR;
		IPR[2] = 6010;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IW62 == 84) {
		IPR[2] = 1040;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IW62 == 83) && ((IXT[320] != 0) || (IR != 52))) {
		IPR[2] = 8031;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IW62 == 83) && (IVEN[48] < 20000)) {
		IPR[2] = 6041;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IW62 == 83) && (IVEN[36] != 0) && (IXT[319] == 4952)) {
		IPR[2] = 6042;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IW62 == 83) && (R > 0.67) && (IXT[319] != 0))  {
		IVEN[36]=152;
		IPR[2] = 6043;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IW62 == 83) {
		IPR[2] = 6044;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IVEN[IW62] != IR) && (IW62 > 47)) {
		IPR[2] = 8031;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IW62 < 54) && (IVEN[IW62] < 20000)) {
		IPR[2] = 8005;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IW62 == 67) {
		IXT[187]=1314;
		IPR[2] = 1095;
		IPR[3] = IR;
		goto MMSI;
	}
	IPR[2] = 8010;
	IPR[3] = 0;
	goto MMSI;
MMSH_7000:
	if ((IW52 != 3) || (IW62 != 73) || (IW71 != 2)) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSI;
	}
	MMRLret=7;
	goto MMRL;
RLret7:
	if (IR != 11) {
		IPR[2] = 8031;
		IPR[3] = 0;
		goto MMSI;
	}
	IC = ITD(IVEN[73]);
	if (IC != 0) {
		IVEN[73] = 1000;
		IPR[2] = 7010;
		IPR[3] = IC;
		goto MMSI;
	}
	IPR[2] = 7090;
	IPR[3] = 0;
	goto MMSI;
MMSH_8000:
	if ((IW71 != 2) || ((IW52 != 4) && (IW62 != 63))) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSI;
	}
	MMRLret=8;
	goto MMRL;
RLret8:
	if ((IR != 1) && (IW62 == 63)) {
		IPR[2] = 2030;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IR == 1) && (IW62 == 63)) {
		IXT[1] = 401;
		IPR[2] = 1095;
		IPR[3] = IR;
		goto MMSI;
	}
	if (IVEN[24] < 20000) {
		IPR[2] = 8006;
		IPR[3] = IR;
		goto MMSI;
	}
	if (IFSD(IRES[IW62]) != IR) {
		IPR[2] = 2030;
		IPR[3] = 0;
		goto MMSI;
	}
	IVEN[24]=309;
	IPR[2] = 8015;
	IPR[3] = 0;
	goto MMSI;
MMSH_9000:
	IPR[2] = 320;
	IPR[3] = 0;
	goto MMSI;
MMSH_10000:
	if (IWRD[0][10] == 1) {
		IPR[2] = 1000;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IWRD[0][10] != 2) || (IW52 != 4)) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSI;
	}
	MMRLret=9;
	goto MMRL;
RLret9:
	/*
	 * If the NPC is not here, say that we don't see him/her
	 */
	IC=IW62;
	if (IFSD(IRES[IC]) != IR) {
		IPR[2] = 2030;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IRES[IC] > 10000) {
		IPR[2] = 20030;
		IPR[3] = 0;
		goto MMSI;
	}
	/*
         * If it's KILL VAMPIRE, the vampire is in the Creepy Crypt,
	 * they've got the wedge and either the club or the axe, and
	 * if they've opened the coffin and the game time in in the first
	 * two hours, then drive the stake through the vampire's heart.
	 */
	if ((IC == 5) && (IR == 5) && (IRES[5] == 5) && (IVEN[29] >= 20000) && 
	   ((IVEN[11] > 20000) || (IVEN[14] > 20000)) && (ITST[5] < 50) && 
	   (IXT[183] == 405)) {
		IPR[2] = 10011;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IC == 5) || (IC == 10)) {
		IPR[2] = 10020;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IC == 15) || (IC == 10)) {
		IPR[2] = 10030;
		IPR[3] = 0;
		goto MMSI;
	}
	IP = IRES[IC]/1000 - ITST[31];
	for (I=1; I<=89; I++) {
		if ((I != 10) && (I != 11) && (I != 12) && (I != 14) && 
		    (I != 34) && (I != 45) && (I != 48) && (I != 49))
			continue;
		if (IVEN[I] > 20000) IP += IFD(IVEN[I]);
		if ((IFSD(IVEN[I]) == IC) && (IVEN[I] > 10000) && 
		    (IVEN[I] < 20000)) IP -= IFD(IVEN[J]);
	}
	R=RN(R);
	IP = (int)(IP+5*R-2);
	if (IP == 6) IP=8;
	if (IP == 7) IP=9;
	if (IP == -6) IP = -8;
	if (IP == -7) IP = -9;
	if (IP > 9) IP = 10;
	if (IP < -9) IP=10;
	if (IP < 0) ITST[31] = -1*IP;
	if (IP > 0) IRES[IC] = IRES[IC] - 1000*(IRES[IC]/1000)+1000*IP;
	if ((abs(IP) < 2) && (IC != 16)) goto MMSH_10050;
	if (IP < -9) {
		IPR[2] = 10030;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IP > 9) && (IC == 13) && (IR == 33)) IRES[13]=534;
	if ((IP > 9) && (IC == 13) && (IR != 33)) IRES[13]=533;
	if ((IP > 9) && (IC == 13)) {
		IPR[2] = 16045;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IP > 9) {
		IPR[2] = 10040;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IP > 0) {
		IPR[2] = 10041;
		IPR[3] = 0;
		goto MMSI;
	}
	IPR[2] = 10031;
	IPR[3] = 0;
	goto MMSI;
MMSH_10050:
	for (J=1; J<=360; J++) {
		if ((IFSD(IXT[J]) != IR) || (IXT[J] > 20000)) continue;
		IRES[IC] = IRES[IC]-IR+ITFD(IXT[J]);
		IPR[2] = 10060;
		IPR[3] = 0;
		goto MMSI;
	}
	R=RN(R);
	if (R < 0.3) {
		IPR[2] = 10030;
		IPR[3] = 0;
		goto MMSI;
	}
	IRES[IC] = 10000+IR;
	IPR[2] = 10040;
	IPR[3] = 0;
	goto MMSI;
MMSH_11000:
	if (IW71 > 1) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSI;
	}
	for (I=1; I<=8; I++) {
		for (J=0; J<8; J++) {
			IVRBX[I][J]=IVRB[3+I][J];
		}
	}
	if (!(fd=open("mmfreeze", O_CREAT | O_WRONLY, 0644))) {
		IPR[2] = 11040;
		IPR[3] = 0;
		goto MMSI;
	}
	/*
	 * Ignoring return values == Bad.  Fix asap.
	 */
	write(fd, IVRBX, sizeof(IVRBX));
	write(fd, IROM, sizeof(IROM));
	write(fd, IRES, sizeof(IRES));
	write(fd, IVEN, sizeof(IVEN));
	write(fd, IXT, sizeof(IXT));
	write(fd, ITST, sizeof(ITST));
	write(fd, IWP, sizeof(IWP));
	IPR[2] = 11050;
	IPR[3] = 0;
	goto MMSI;
MMSH_12000:
	if (IW71 > 1) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSI;
	}
	if (ITST[5] > 1) {
		IPR[2] = 12050;
		IPR[3] = 0;
		goto MMSI;
	}
	IXX = ITST[19];
	if (!(fd=open("mmfreeze", O_RDONLY, 0))) {
		IPR[2] = 12070;
		IPR[3] = 0;
		goto MMSI;
	}
	read(fd, IVRBX, sizeof(IVRBX));
	read(fd, IROM, sizeof(IROM));
	read(fd, IRES, sizeof(IRES));
	read(fd, IVEN, sizeof(IVEN));
	read(fd, IXT, sizeof(IXT));
	read(fd, ITST, sizeof(ITST));
	read(fd, IWP, sizeof(IWP));
	for (I=1; I<=8; I++) {
		for (J=0; J<8; J++) {
			IVRB[3+I][J]=IVRBX[I][J];
		}
	}
	ourtime(ITIM);
	ITST[19]=IXX;
	ITST[18]=(int)(ITIM[4]*1200+ITIM[3]*20+ITIM[2]/3);
	ITST[20]=ITST[20]+1;
	IPR[2] = 12040;
	IPR[3] = 0;
	goto MMSI;
MMSH_13000:
	if ((IW71 != 1) && (IW62 != 13) && (IW62 != 60) && (IW62 != 62)) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IW71 > 2) {	
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IW71 == 1) IW62 = 62;
	if ((IW62 == 60) && (IR != 16)) {
		IPR[2] = 2030;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IW62 == 60) {
		IXT[128] = 1316;
		IPR[2] = 1095;
		IPR[3] = IR;
		goto MMSI;
	}
	if ((IW62 == 13) && ((IVEN[13] < 20000) || (ITST[10] == 13))) {
		IPR[2] = 2030;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IW62 == 13) {
		IPR[2] = 13010;
		IPR[3] = 0;
		goto MMSD;
	}
	if ((IW62 == 62) && ((IR < 68) || (IR > 83)) && (IR != 99)) {
		IPR[2] = 2030;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IW62 == 62) { 
		IPR[2] = 1010;
		IPR[3] = 0;
		goto MMSI;
	}
	IPR[2] = 320;
	IPR[3] = 0;
	goto MMSI;
MMSH_14000:
	if (IW71 > 1) goto MMSH_14100;
	if (ITST[5] > 2) {
		IPR[2] = 14010;
		IPR[3] = 0;
		goto MMSI;
	}
MMSH_14020:
	printf("\nMYSTERY #? "); 
	fgets(buf, sizeof(buf)-1, stdin);
	buf[sizeof(buf)-1] = 0x00;
	IC = atoi(buf);
	if (!IC || (IC < 0)) goto MMSH_14020;
	if (IC > 999) goto MMSH_14020;
	RND = 0.001*(double)IC;
	printf("\nTHIS GAME IS NOW SET UP FOR MYSTERY #%d\n", IC);
	fflush(stdout);
	ITST[23]=IC;
	IPR[2]=0;
	IPR[3]=0;
	goto MMSA;
MMSH_14100:
	if ((IWRD[0][10] > 2) || (IWRD[1][9] != 107)) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSI;
	}
	IPR[2] = 14110;
	IPR[3] = 0;
	goto MMSI;
MMSH_15000:
	if ((IW71 != 2) || (IW52 != 4)) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IFSD(IRES[IW62]) != IR) {
		IPR[2] = 2030;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IVEN[7] < 20000) {
		IPR[2] = 15200;
		IPR[3] = 0;
		goto MMSI;
	}
	if (IRES[IW62] > 10000) {
		IPR[2] = 15400;
		IPR[3] = 0;
		goto MMSI;
	}
	IC=0;
	IG=IW62;
	IM=ITST[9];
	IR=ITST[8];
	if ((IM == 2) && (IR > 53) && ((IG == 7) || (IG == 8) || (IG == 9)))
		IC=1;
	if ((IM == 2) && (IR < 28) && ((IG == 3) || (IG == 8) || (IG == 14)))
		IC=1;
	if ((IM == 3) && (IR > 18) && ((IG == 2) || (IG == 8) || (IG == 14)))
		IC=1;
	if ((IM == 3) && (IR < 10) && ((IG == 7) || (IG == 9) || (IG == 14)))
		IC=1;
	if ((IM == 7) && (IR > 53) && ((IG == 2) || (IG == 8) || (IG == 9)))
		IC=1;
	if ((IM == 7) && (IR < 10) && ((IG == 3) || (IG == 9) || (IG == 14)))
		IC=1;
	if ((IM == 8) && (IR > 53) && ((IG == 2) || (IG == 7) || (IG == 9)))
		IC=1;
	if ((IM == 8) && (IR < 28) && ((IG == 2) || (IG == 3) || (IG == 14)))
		IC=1;
	if ((IM == 9) && (IR > 53) && ((IG == 2) || (IG == 7) || (IG == 8)))
		IC=1;
	if ((IM == 9) && (IR < 10) && ((IG == 3) || (IG == 7) || (IG == 14)))
		IC=1;
	if ((IM == 14) && (IR > 9) && ((IG == 2) || (IG == 3) || (IG == 8)))
		IC=1;
	if ((IM == 14) && (IR < 10) && ((IG == 3) || (IG == 7) || (IG == 9)))
		IC=1;
	if (IC == 0) {
		IPR[2] = 15400;
		IPR[3] = 0;
		goto MMSI;
	}
	IVEN[7] = 0;
	IRES[IG] = 0;
	IPR[2] = 15500;
	IPR[3] = 0;
	goto MMSI;

	/*
	 * QUESTION routine
	 */
MMSH_16000:
	/* if only one word in input then complain */
	if (IWRD[0][10] == 1) {
		IPR[2] = 1000;
		IPR[3] = 0;
		goto MMSI;
	}
	/* if more than two words and the second is not an npc, then complain */
	if ((IWRD[0][10] != 2) || (IWRD[1][8] != 4)) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSI;
	}
	/* make sure the player is in the same room as the npc */
	if (IFSD(IRES[IWRD[1][9]]) != IR) {
		IPR[2] = 2030;
		IPR[3] = 0;
		goto MMSI;
	}

	/* make sure npc is alive and vocal */
	if ((IRES[IWRD[1][9]] >= 1000) || (IWRD[1][9] == 4) || 
	    (IWRD[1][9] == 15)) {
		IPR[2] = 20030;
		IPR[3] = 0;
		goto MMSI;
	}

	/* make the npc irritated */
	R=RN(R);
	if (((ITST[5]-2) > ITST[13]) && (R < 0.5)) {
		IPR[2] = 23120;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((ITST[5]-4) > ITST[13]) {
		IPR[2] = 23145;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((ITST[5]-2) > ITST[13]) {
		IPR[2] = 23140;
		IPR[3] = 0;
		goto MMSI;
	}

	/* vampire */
	if (IWRD[1][9] == 5) {
		IPR[2] = 23010;
		IPR[3] = 0;
		goto MMSI;
	}

	/* werewolf */
	if (IWRD[1][9] == 10) {
		IPR[2] = 23015;
		IPR[3] = 0;
		goto MMSI;
	}

	/* elf and dwarf */
	if ((IWRD[1][9] == 11) || (IWRD[1][9] == 13)) {
		IPR[2] = 23020;
		IPR[3] = 0;
		goto MMSI;
	}
	
	/* woodsman and hunter */
	if ((IWRD[1][9] != 6) && (IWRD[1][9] != 12)) goto MMSH_23130;
	if ((IXT[54] > 20000) && (IR != 93) && (R < 0.3)) {
		IPR[2] = 23030;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IXT[54] > 20000) && (IR != 93) && (R < 0.3)) {
		IPR[2] = 23040;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IXT[54] < 20000) && (R < 0.3)) {
		IPR[2] = 23050;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.4) {	
		IPR[2] = 23060;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.5) {	
		IPR[2] = 23070;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.58) {	
		IPR[2] = 23080;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.66) {	
		IPR[2] = 23090;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.74) {	
		IPR[2] = 23100;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.82) {	
		IPR[2] = 23110;
		IPR[3] = 0;
		goto MMSI;
	}
	IPR[2] = 23120;
	IPR[3] = 0;
	goto MMSI;

	/* murderer */
MMSH_23130:
	if (IWRD[1][9] != ITST[9]) goto MMSH_23160;
	if (R < 0.33) {	
		IPR[2] = 23120;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.67) {	
		IPR[2] = 23140;
		IPR[3] = 0;
		goto MMSI;
	}
	IPR[2] = 23150;
	IPR[3] = 0;
	goto MMSI;

	/* master and butler */
MMSH_23160:
	if ((IWRD[1][9] != 3) && (IWRD[1][9] != 14)) goto MMSH_23250;
	if (R < 0.3) goto MMSH_23200;
	if ((ITST[8] < 28) && (R < 0.15)) {
		IPR[2] = 23170;
		IPR[3] = 0;
		goto MMSI;
	}
	if (ITST[8] < 28) {
		IPR[2] = 23180;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IVEN[ITST[10]] < 10000) && (IFSD(IVEN[ITST[10]]) < 28)) {
		IPR[2] = 23190;
		IPR[3] = 0;
		goto MMSI;
	}
	IPR[2] = 23150;
	IPR[3] = 0;
	goto MMSI;
MMSH_23200:
	if (R < 0.4) {	
		IPR[2] = 23210;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.5) {	
		IPR[2] = 23220;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.6) {	
		IPR[2] = 23230;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.7) {	
		IPR[2] = 23240;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.8) {	
		IPR[2] = 23140;
		IPR[3] = 0;
		goto MMSI;
	}
	IPR[2] = 23120;
	IPR[3] = 0;
	goto MMSI;

	/* lady and maid */
MMSH_23250:
	if ((IWRD[1][9] != 8) && (IWRD[1][9] != 2)) goto MMSH_23310;
	if (R < 0.3) goto MMSH_23260;
	if ((ITST[8] > 9) && (R < 0.15)) {
		IPR[2] = 23170;
		IPR[3] = 0;
		goto MMSI;
	}
	if (ITST[8] > 9) {
		IPR[2] = 23180;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IVEN[ITST[10]] < 10000) && (IFSD(IVEN[ITST[10]]) > 9)) {
		IPR[2] = 23190;
		IPR[3] = 0;
		goto MMSI;
	}
	IPR[2] = 23150;
	IPR[3] = 0;
	goto MMSI;
MMSH_23260:
	if (R < 0.4) {	
		IPR[2] = 23270;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.5) {	
		IPR[2] = 23280;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.6) {	
		IPR[2] = 23290;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.7) {	
		IPR[2] = 23300;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.8) {	
		IPR[2] = 23140;
		IPR[3] = 0;
		goto MMSI;
	}
	IPR[2] = 23150;
	IPR[3] = 0;
	goto MMSI;

	/* cook and gardener */
MMSH_23310:
	if (R < 0.3) goto MMSH_23320;
	/*
	 * This is a nasty piece of code.  There were no parens in
	 * the original, so operator precedence prevailed -- but it's
	 * not clear what the precedence in Fortran IV was.  Because the
	 * murder scene (ITST[8]) is cleared to zero when the murder is solved,
	 * it seems like we definitely wouldn't want to get to the next
	 * test and output (ie, 23180, just below) because it could result
	 * in dereferencing a null pointer.  So I have added parens to make
	 * it clear that if ITST < 10, we go here.  However, it doesn't
	 * make much sense to put the same test below, then... this could
	 * just be the result of slightly sloppy coding on Bill's part,
	 * but it should be eventually resolved will all the other paren
	 * problems.  This bug was found by GianCarlo.
	 */
	if ((ITST[8] < 10) || ((ITST[8] > 27) && (R < 0.15))) {
		IPR[2] = 23170;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((ITST[8] < 10) || (ITST[8] > 27)) {
		IPR[2] = 23180;
		IPR[3] = 0;
		goto MMSI;
	}
	if ((IVEN[ITST[10]] < 10000) && (IFSD(IVEN[ITST[10]]) > 9)) {
		IPR[2] = 23190;
		IPR[3] = 0;
		goto MMSI;
	}
	IPR[2] = 23150;
	IPR[3] = 0;
	goto MMSI;
MMSH_23320:
	if (R < 0.4) {	
		IPR[2] = 23330;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.5) {	
		IPR[2] = 23340;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.6) {	
		IPR[2] = 23350;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.7) {	
		IPR[2] = 23360;
		IPR[3] = 0;
		goto MMSI;
	}
	if (R < 0.8) {	
		IPR[2] = 23140;
		IPR[3] = 0;
		goto MMSI;
	}
	IPR[2] = 23150;
	IPR[3] = 0;
	goto MMSI;

	/*
	 *
	 */
MMSH_17000:
	if (IWRD[0][10] != 1) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSI;
	}
	IX=IC=0;
	ID=24000+IR;
	if (IR > 53) ID=24100;
	if ((IVEN[3] > 20000) || (IVEN[3] == (200+IR)) || 
	    ((IVEN[3] > 10000) && (IFSD(IVEN[3]) == IR))) IX=24108;
	for (I=2; I<=15; I++) {
		if ((IRES[I] > 10000) || (IFSD(IRES[I]) != IR)) continue;
		IX=24110;
		if ((I != 4) && (I != 10) && (I != 15)) IC += 1;
		if (IC == 1) IX=24120;
		if (IC < 2) continue;
		if (IC == 2) IX=24130;
		if (IC == 3) IX=24140;
	}
	if (((IRES[6]+IRES[12]) < 10000) && (IFSD(IRES[6]) == IR) &&
	    (IFSD(IRES[12]) == IR)) IX=24150;
	if (((IRES[3]+IRES[14]) < 10000) && (IFSD(IRES[3]) == IR) &&
	    (IFSD(IRES[14]) == IR) && (ITST[9] != 3) && (ITST[9] != 14) &&
	    (ITST[8] < 28)) 
		IX=24160;
	RNX=77.7*(double)IR/253.0-(double)(77.7*(double)IR/253.0);
	if ((IX == 24160) && (RNX > 0.8)) IX=24164;
	if ((IX == 24160) && (RNX > 0.6)) IX=24162;
	if (((IRES[7]+IRES[9]) < 10000) && (IFSD(IRES[7]) == IR) &&
	    (IFSD(IRES[9]) == IR) && (ITST[9] != 7) && (ITST[9] != 9) &&
	    ((ITST[8] < 10) || (ITST[8] > 27)) )
		IX=24170;
	if ((IX == 24170) && (RNX > 0.8)) IX=24174;
	if ((IX == 24170) && (RNX > 0.6)) IX=24172;
	if (((IRES[2]+IRES[8]) < 10000) && (IFSD(IRES[2]) == IR) &&
	    (IFSD(IRES[8]) == IR) && (ITST[9] != 2) && (ITST[9] != 8) &&
	    (ITST[8] > 27))
		IX=24180;
	if ((IX == 24180) && (RNX > 0.8)) IX=24184;
	if ((IX == 24180) && (RNX > 0.6)) IX=24182;
	if ((ID == 24100) && (IX != 0)) ID=24105;
	IPR[2] = ID;
	IPR[3] = IX;
	goto MMSI;
MMSI:
	IRIT=IPR[2];
	IW63=IWRD[2][9];
	IFC=ITST[4]/1000;
	IDN=1;
	IDE=2;
	IDS=3;
	IDW=4;
	if (IVEN[6] < 20000) IDN=10-IFC;
	if (IDN == 6) IDN=10;
	if (IVEN[6] < 20000) IDE=11-IFC;
	if (IVEN[6] < 20000) IDS=12-IFC;
	if (IDS == 11) IDS=7;
	if (IVEN[6] < 20000) IDW=13-IFC;
	if (IDW > 10) IDW=IDW-4;
	
	if (IRIT == 320) 
		printf("\nI CANNOT FIGURE OUT WHAT YOU ARE TRYING TO SAY");
	if (IRIT == 520) 
		printf("\nNOTHING HAPPENS");
	if (IRIT == 1000) {
		printf("\nPLEASE TRY AGAIN. I NEED TO KNOW WHO OR WHAT TO ");
		for (L=0; L<8; L++)
			putchar(IWRD[0][L]);
	}
	if (IRIT == 1010) 
		printf("\nTHANKS. THAT TASTED GOOD.");
	if (IRIT == 1020) 
		printf("\nUGH! THAT TASTED AWFUL.");
	if (IRIT == 1040) 
		printf("\nOKAY");
	if (IRIT == 1065) 
		printf("\nPROGRAM ERROR AT LINE %d", IPR[3]);
	if ((IPR[3] == 7) || (IPR[3] == 16) || (IPR[3] == 25)) IC = IDS;
	if ((IPR[3] == 5) || (IPR[3] == 14) || (IPR[3] == 23)) IC = IDW;
	if ((IPR[3] == 1) || (IPR[3] == 10) || (IPR[3] == 19)) IC = IDN;
	if (IRIT == 1095) {
		printf("\nYOUR ACTION OPENED A PANEL INTO THE SECRET PASSAGE "
			"THAT YOU CAN\nENTER BY GOING ");
		for (I=0; I<8; I++)
			putchar(IDTN[IC][I]);
	}
	if (IRIT == 1150) {
		printf("\nIT IS DARK HERE\n");
		if (IPR[3] == 1)
			printf("\nIT IS AFTER SUNSET");
		if (IPR[3] == 2)
			printf("\nA CURTAIN BLOCKS THE LIGHT");
	}
	if (IRIT == 2010) {
		printf("\nYOU CANNOT ");
		for (L=0; L<8; L++)
			putchar(IWRD[0][L]);
		printf(" THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[1][L]);
	}
	if (IRIT == 2030) {
		printf("\nI DON'T SEE THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[1][L]);
	}
	if (IRIT == 2037) 
		printf("\nYOU MUST REALIZE BY NOW THAT THE DELICATE PAPYRUS "
			"WILL CRUMBLE WHEN\nYOU SO MUCH AS EVEN BLOW ON IT.");
	if (IRIT == 2080) 
		printf("\nYOU JUST GOT SLAPPED BY THE MAID");
	if (IRIT == 2090) 
		printf("\nYOU ARE NOT ALLADIN. BESIDES, YOU KNOW HOW MUCH "
			"TROUBLE HE GOT INTO");
	if (IRIT == 3010) {
		printf("\nYOU DO NOT HAVE THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[1][L]);
	}
	if (IRIT == 9040) ITST[34]=(int)(500.1*RND);
	if (IRIT == 3050) 
		printf("\nYOU NEED A MATTER TRANSMITTER TO BEAM UP");
	if (IRIT == 3060) 
		printf("\nYOU DEMATERIALIZE IN SECONDS AND REMATERIALIZE "
			"IN THE LABORATORY");
	if (IRIT == 3070) 
		printf("\nYOU HAVE TO BE IN THE LABORATORY TO BEAM DOWN");
	if (IRIT == 3080) 
		printf("\nTHE MATTER RECEIVER HAS TO BE SOMEWHERE ELSE TO "
			"BEAM DOWN");
	if (IRIT == 3090) {
		printf("\nYOU DEMATERIALIZE IN SECONDS AND REMATERIALIZE IN "
			"THE ");
		for (L=0; L<16; L++)
			putchar(IRNM[IFSD(IROM[IPR[3]])][L]);
	}
	if (IRIT == 3110) {
		printf("\nYOU NO LONGER HAVE THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[1][L]);
	}
	if (IRIT == 4010) {
		printf("\nFIRST YOU HAVE TO FIND THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[3][L]);
	}
	if (IRIT == 4050) 
		printf("\nTHANKS. IT NEEDS THAT AROUND HERE.");
	if (IRIT == 4055) 
		printf("\nYOU DON'T HAVE A BROOM OR ANYTHING TO SWEEP WITH");
	if (IRIT == 5020) {
		printf("\nI DO NOT SEE THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[2][L]);
	}
	if (IRIT == 5001) {
		printf("\nTHE POLICE CAME AND ARRESTED THE MURDERER");
		if (ITST[24] != 0) {
			ITST[1] = 8;
			if (IVEN[2] > 20000) IVEN[2] = 0;
			if (IVEN[9] > 20000) IVEN[9] = 0;
			printf("\nSINCE YOU KILLED AN INNOCENT BYSTANDER, "
				"THE POLICE LOCKED YOU IN\nTHE DUNGEON.");
		}
	}
	if (IRIT == 5002) 
		printf("\nTHE DISPATCHER SAID THAT A TAXI WOULD BE THERE IN "
			"ABOUT 2 HOURS.");
	if (IRIT == 5009) 
		printf("\nALL YOU CAN GET IS A BUSY SIGNAL");
	if (IRIT == 6005) {
		printf("\nA DOZEN OR SO OF THE INDIANS AIMING IN YOUR "
			"DIRECTION, SHOT THEIR\nARROWS AND YOU WERE HIT BY "
			"SEVERAL OF THEM AND DIED A PAINFUL DEATH.");
		IPR[2] = 6005;
		IPR[3] = 0;
		goto MMSD;
	}
	if (IRIT == 6010) 
		printf("\nYOU JUST UNCOVERED SOME GOLD COINS.");
	if (IRIT == 6015) {
		printf("\nAFTER MOVING THE TABLE, A TRAP DOOR OPENED "
			"REVEALING A GAUDY GAUNTLET.");
		IVEN[48]=9115;
	}
	if (IRIT == 6030) 
		printf("\nUNDER THE RAGS IS A STRANGE DRAWING OF A COMPASS "
			"WITH THE NEEDLE BENT\nIN A U-SHAPE AND A CLOCK WITH "
			"TWO MINUTE HANDS, ONE ON THE 10 AND THE OTHER\nON "
			"THE 12");
	if (IRIT == 6040) 
		printf("\nUNDER THE RAGS IS A STRANGE DRAWING OF A COMPASS "
			"WITH THE NEEDLE\nPOINTING NORTH AND A CLOCK WITH "
			"ONLY A MINUTE HAND WHICH IS ON THE 12");
	if (IRIT == 6041) 
		printf("\nYOU CANNOT QUITE MOVE ANY OF THE BOULDERS.");
	if (IRIT == 6042) {
		printf("\nIN YOUR GREED TO FIND MORE LOOT, YOU MANAGED TO "
			"MOVE ONE OF THE BOULDERS\nIN THE WRONG DIRECTION AND "
			"IT ROLLED INTO AND BLOCKED THE ONLY EXIT.");
		IXT[319]=0;
	}
	if (IRIT == 6043) 
		printf("\nWITH A HEAVY PUSH, YOU MOVED ONE OF THE BOULDERS "
			"A FEW FEET AND\nUNCOVERED A BAG OF PEARLS IN ONE OF "
			"THE CRATERS THAT WAS UNDER IT.");
	if (IRIT == 6044) 
		printf("\nTHANKS TO THE GAUNTLET, YOU WERE ABLE TO MOVE A "
			"BOULDER A FEW FEET\nBUT THERE WAS NOTHING UNDER IT.");
	if (IRIT == 7010) {
		printf("\nA VOICE ON THE LINE SAYS:");
		IVEN[42]=1000;
		if (IPR[3] == 1)
			printf("\nWHY DON'T YOU DROP THE NOTE? YOU DON'T NEED "
				"IT ANYMORE.");
		if (IPR[3] == 2)
			printf("\nWHY DON'T YOU LOOK AT SOME OF THE ITEMS YOU "
				"HAVE");
		if (IPR[3] == 3)
			printf("\nIF I WERE YOU, I WOULD WATCH THE MURDERER "
				"AND CALL THE POLICE.");
		if (IPR[3] == 4)
			printf("\nIT WOULD PROBABLY BE WISE TO GO BACK TO THE "
				"MAIN GATE AND GET\nTHE LANTERN BEFORE YOU "
				"GO MUCH FARTHER.");
		printf(" AND HANGS UP");
	}
	if (IRIT == 7090)
		printf("\nTHERE IS NO ONE ON THE LINE");
	if (IRIT == 7050)
		printf(" NOTHING");
	if (IRIT == 8005) {
		printf("\nYOU DO NOT HAVE THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[3][L]);
	}
	if (IRIT == 8006)
		printf("\nYOU DO NOT HAVE ANY FOOD.");
	if (IRIT == 8010)
		printf("\nI DON'T KNOW WHAT YOU EXPECT TO HAPPEN");
	if (IRIT == 8015) {
		printf("\nYOUR FOOD HAS BEEN EATEN BY THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[1][L]);
	}
	if (IRIT == 8020)
		printf("\nIT ALREADY IS");
	
	if (IRIT == 8030) {
		printf("\nI DON'T SEE THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[2][L]);
	}
	if (IRIT == 8031) {
		printf("\nI DON'T SEE THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[1][L]);
	}
	if (IRIT == 8032) 
		printf("\nI DON'T SEE HOW YOU CAN DO THAT.");
	if (IRIT == 8035) 
		printf("\nTHE BATTERY IN YOUR LANTERN IS DEAD");
	if (IRIT == 9027) 
		printf("\nYOU DON'T HAVE ENOUGH POINTS");
	if (IRIT == 9035) {
		ITST[25] = 10;
		printf("\nGIVE UP.\nYOU WILL NEVER FIGURE IT OUT.");
	}
	if (IRIT == 10020) 
		printf("\nYOU HAVE NOT FIGURED OUT HOW TO DO THAT YET.");
	if (IRIT == 10030) {
		printf("\nYOU WERE KILLED IN THE STRUGGLE");
		IPR[2] = 10030;
		IPR[3] = 0;
		goto MMSD;
	}
	if (IRIT == 10031)
		printf("\nYOU WERE WOUNDED IN THE FIGHT.");
	if (IRIT == 10035)
		printf("\nTHE DOOR IS LOCKED");
	if (IRIT == 10040) {
		printf("\n\nYOU KILLED THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[1][L]);
	}
	if (IRIT == 10041) {
		printf("\n\nYOU WOUNDED THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[1][L]);
	}
	if (IRIT == 10045)
		printf("\nTHE DOOR IS OPEN");
	if (IRIT == 10046)
		printf("\nTHE DOOR IS CLOSED");
	if (IRIT == 10060)
		printf("\nYOUR OPPONENT FLED DURING THE STRUGGLE");
	if (IRIT == 10011) {
		printf("\nYOU JUST DROVE A STAKE INTO THE VAMPIRE'S HEART AND "
			"HE HAS AGED\nA THOUSAND YEARS AND DISAPPEARED.");
		IRES[5] = 0;
		IVEN[1] = 105;
		IVEN[29] = 0;
		ITST[2] += 75;
	}
	if (IRIT == 11040) 
		perror("\nI WAS UNABLE TO SAVE YOUR SITUATION: "
			"open(\"mmfreeze\", O_WRONLY | O_CREAT)");
	if (IRIT == 11050) 
		printf("\nYOUR CURRENT SITUATION HAS BEEN SAVED. YOU MAY "
			"RESUME THIS MYSTERY\nAT A LATER TIME BY ENTERING "
			"RESTORE DURING YOUR FIRST MOVE OF A FUTURE GAME.");
	if ((IRIT == 12040) && (ITST[15] == 17))
		printf("\nYOU ARE NOW IN THE SAME SITUATION YOU WERE IN "
			"WHEN YOU SUSPENDED\n");
	if ((ITST[15] != 17) && (IRIT == 12040)) 
		printf("\nTHE DATA YOU LOADED IS FOR A DIFFERENT REVISION");
	if (IRIT == 12060)
		printf("\nERROR %d WHILE RESTORING FROZEN GAME", IPR[3]);
	if (IRIT == 14010)
		printf("\nYOU CAN SET THE MYSTERY ON THE FIRST MOVE. YOU ARE "
			"IN MYSTERY #%d", ITST[23]);
	if (IRIT == 12070)
		printf("\nI WAS UNABLE TO FIND YOUR OLD GAME FILE!");
	if (IRIT == 14110)
		printf("\nTHAT IS THE NAME OF THE GAME.");
	if (IRIT == 15110)
		printf("\nYOUR KEY WILL NOT FIT THE PADLOCK");
	if (IRIT == 15200) {
		printf("\nYOU DO NOT HAVE ANYTHING OF INTEREST TO THE ");
		for (L=0; L<8; L++)
			putchar(IRSN[IWRD[1][9]][L]);
	}
	if (IRIT == 15400) {
		printf("\nYOU CAN FIND OUT NOTHING FROM THE ");
		for (L=0; L<8; L++)
			putchar(IRSN[IWRD[1][9]][L]);
	}
	if ((IRIT == 15500) && (ITST[8] != 0)) {
		printf("\nAFTER A SLIGHT PAUSE BUT TAKING YOUR TREASURE, THE ");
		for (L=0; L<8; L++)
			putchar(IRSN[IWRD[1][9]][L]);
		printf("\nTELLS YOU THAT THE SCENE OF THE MURDER IS THE ");
		for (L=0; L<16; L++)
			putchar(IRNM[IFSD(IROM[ITST[8]])][L]);
		printf("\nAND THAT THE MURDER WAS COMMITTED BY THE ");
		for (L=0; L<8; L++)
			putchar(IRSN[ITST[9]][L]);
		printf("\nWITH THE %s", IWP);
	}
	if ((IRIT == 15500) && (ITST[8] == 0)) {
		printf("\nAFTER A SLIGHT PAUSE BUT TAKING YOUR TREASURE, "
			"THE ");
		for (L=0; L<8; L++)
			putchar(IRSN[IWRD[1][9]][L]);
		printf("\nTELLS YOU THAT YOU MIGHT HAVE TO EXIT THROUGH THE "
			"ATTIC.");	
	}
	if (IRIT == 16035) {
		printf("\nYOU JUST SMASHED THE PADLOCK TO PIECES.");
		IVEN[35] += 1000;
	}
	if (IRIT == 16045) {
		printf("\nYOU KILLED THE DWARF AND HE DISAPPEARED IN A "
			"CLOUD OF GREASY BLACK SMOKE");
		IRES[13] += 100;
	}
	if (IRIT == 20010)
		printf("\nYOU NEED AN OILCAN OR SOMETHING");
	if (IRIT == 20020)
		printf("\nYOUR OILCAN IS EMPTY");
	if (IRIT == 20030)
		printf("\nDON'T BE SILLY");
	if (IRIT == 22010)
		printf("\nTHE CHEST IS NOT WHERE YOU CAN BREAK THE PADLOCK");
	if (IRIT == 22020)
		printf("\nYOU DO NOT HAVE ANYTHING THAT WILL BREAK THE "
			"PADLOCK");
	if (IRIT == 23010)
		printf("\nTHE VAMPIRE SAYS THAT HE IS COUNT DRACULA AS HE "
			"WALKS SLOWLY TOWARDS YOU,\nHOLDING YOU MOTIONLESS "
			"IN HIS WIDE EYED GAZE.");
	if (IRIT == 23015)
		printf("\nTHE BIG UGLY THING JUST GRUNTS AS HE WALKS "
			"TOWARDS YOU.");
	if (IRIT == 23020)
		printf("\nTHE EVIL LITTLE THING JUST LOOKS UP AT YOU "
			"AND LAUGHS.");
	if (IRIT == 23030)
		printf("\nHE SAYS THAT HE WILL FOLLOW YOU TO THE MAIN "
			"GATE AND HELP YOU OPEN IT, IF THAT IS WHAT YOU "
			"WANT.");
	if (IRIT == 23040)
		printf("\nHE SAYS THAT THE GATE IS HEAVY AND HE WILL HELP "
			"YOU OPEN IT,\nIF YOU TELL YOUR INVISIBLE GUIDE: "
			"OPEN THE GATE.");
	if (IRIT == 23050)
		printf("\nHE SAYS THAT HE ALWAYS CLOSES THE FRONT GATE "
			"WHEN HE IS\nTHERE WITH A FRIEND.");
	if (IRIT == 23060)
		printf("\nHE SAYS THAT YOU CAN READ THE MAP THAT IS IN THE "
			"COTTAGE.");
	if (IRIT == 23070)
		printf("\nHE SAYS TO WATCH OUT FOR THE 500 FOOT SHEER CLIFF "
			"AROUND THE\nEASTERN SIDE OF THE MANSION.");
	if (IRIT == 23080)
		printf("\nHE SAYS THAT HE IS AFRAID TO GO INTO THE COTTAGE.");
	if (IRIT == 23090)
		printf("\nHE SAYS THAT HE KNOWS BETTER THAN TO GO INTO THE "
			"DENSE WOODS.");
	if (IRIT == 23100)
		printf("\nHE SAYS THAT HE IS NOT ALLOWED TO GO INTO THE "
			"GARDEN.");
	if (IRIT == 23110)
		printf("\nTHE RESPONSE IS THAT THE FRONT PORCH IS DANGEROUS.");
	if (IRIT == 23120) {
		printf("\nYOUR QUESTIONS ARE STARTING TO IRRITATE THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[1][L]);
	}
	if (IRIT == 23140)
		printf("\nYOUR LAST QUESTION FELL ON DEAF EARS.");
	if (IRIT == 23145) {
		printf("\nYOUR QUESTIONS ARE REALLY BOTHERING THE ");
		for (L=0; L<8; L++)
			putchar(IRSN[IWRD[1][9]][L]);
	}
	if (IRIT == 23150) {
		printf("\nI KNOW NOTHING ABOUT THE MURDER. ANSWERS THE ");
		for (L=0; L<8; L++)
			putchar(IWRD[1][L]);
	}
	if ((IRIT >= 23170) && (IRIT <= 23190)) 
		printf("\nYOUR QUESTION IS ANSWERED WITH A HESITANT:");
	if (IRIT == 23170) {
		printf("\nI THINK THE MURDERER IS THE ");
		for (L=0; L<8; L++)
			putchar(IRSN[ITST[9]][L]);
	}
	if (IRIT == 23180) {
		printf("\nI THINK THE MURDER WAS COMMITTED IN THE ");
		for (L=0; L<16; L++)
			putchar(IRNM[IFSD(IROM[ITST[8]])][L]);
	}
	if (IRIT == 23190) 
		printf("\nI THINK THE MURDER WEAPON IS THE %s", IWP);
	if (IRIT == 23210) 
		printf("\nHE SAYS THAT HE DOESN'T WANT TO GO OUT OF THE "
			"MANSION.");
	if (IRIT == 23220) 
		printf("\nHE SAYS THAT THERE IS AN ENTRACE TO A SECRET "
			"LABORATORY OFF THE LARGE BEDROOM.");
	if (IRIT == 23230) 
		printf("\nHE SAYS THAT A SECRET PASSAGE CONNECTS ALL THREE "
			"LEVELS OF THE MANSION.");
	if (IRIT == 23240) 
		printf("\nHE SAYS THAT THE MAID CAN BE VERY FRIENDLY.");
	if (IRIT == 23270) 
		printf("\nSHE SAYS SHE IS AFRAID TO GO BELOW THE MAIN FLOOR "
			"OF THE MANSION.");
	if (IRIT == 23280) 
		printf("\nSHE SAYS THAT SHE WILL NOT LEAVE THE MANSION AND "
			"GARDEN.");
	if (IRIT == 23290) 
		printf("\nSHE SAYS THAT SHE CAN READ THE WRITING ON THE "
			"CHEST.");
	if (IRIT == 23300) 
		printf("\nSHE SAYS THAT THE ELF HIDES THINGS IN THE WELL.");
	if (IRIT == 23330) 
		printf("\nHE SAYS THAT THERE IS TREASURE AT THE END OF THE "
			"CORRIDOR.");
	if (IRIT == 23340) 
		printf("\nHE SAYS THAT YOU CAN DISTRACT THE WOLF WITH FOOD.");
	if (IRIT == 23350) 
		printf("\nHE SAYS THAT YOU CAN TELL THE WELLS APART BY "
			"DROPPING ITEMS NEAR THEM.");
	if (IRIT == 23360) 
		printf("\nHE SAYS THAT YOU CAN READ THE SCROLL BY USING "
			"MAGIC WORDS.");
	if (IRIT == 24001) 
		printf("\nYOU CAN HEAR THE CHATTER OF THE MONKEY.");
	if (IRIT == 24002) 
		printf("\nYOU CAN HEAR THE ROAR OF THE FIRE IN THE FURNACE.");
	if (IRIT == 24003) 
		printf("\nYOU CAN HEAR STRANGE SOUNDS COMING UP FROM BELOW "
			"YOU.");
	if ((IRIT == 24004) || (IRIT == 24013) || (IRIT == 24022)) 
		printf("\nYOU CAN HEAR THE CREAKING OF WOOD AS YOU MOVE "
			"ABOUT.");
	if (IRIT == 24005) 
		printf("\nIT IS DEATHLY QUIET HERE.");
	if (IRIT == 24006) 
		printf("\nYOU CAN HEAR SOMEONE WALKING IN THE ROOM ABOVE YOU.");
	if (IRIT == 24007) 
		printf("\nYOU CAN ONLY HEAR THE NOISE YOU MAKE BUMPING "
			"INTO THINGS.");
	if (IRIT == 24008) 
		printf("\nYOU CAN HEAR THE SQUEAKING OF SEVERAL RATS AS "
			"THEY SCURRY ABOUT.");
	if (IRIT == 24009) 
		printf("\nYOU CAN HEAR SOME KIND OF ANIMAL SCRATCHING TO "
			"GET IN.");
	if (IRIT == 24010) 
		printf("\nYOU CAN HEAR BUT REALLY SENSE THE SOFT COMFORT"
			"ABLE SURROUNDINGS.");
	if (IRIT == 24011) 
		printf("\nYOU CAN HEAR A SLIGHT ECHO HIGH ABOVE YOU.");
	if ((IRIT == 24012) && (IFD(IVEN[75]) == 1))
		printf("\nYOU CAN HEAR THE RADIO PLAYING.");
	if ((IRIT == 24012) && (IFD(IVEN[75]) != 1))
		printf("\nYOU CAN HEAR WHAT SOUNDS LIKE THUNDER IN THE "
			"MOUNTAINS THAT ARE NEARBY.");
	if ((IRIT == 24014) && (IFD(IVEN[89]) == 1))
		printf("\nYOU CAN HEAR THE ROAR OF THE FIRE IN THE FIREPLACE.");
	if ((IRIT == 24014) && (IFD(IVEN[89]) != 1))
		printf("\nYOU CAN HEAR YOUR ECHO AS YOU WALK ABOUT THE ROOM.");
	if (IRIT == 24015)
		printf("\nYOU CAN HEAR SOMEONE MAKING NOISE BELOW THE FLOOR.");
	if (IRIT == 24016)
		printf("\nYOU CAN HEAR SOFT ORGAN MUSIC.");
	if (IRIT == 24017)
		printf("\nYOU CAN HEAR THE FAINT SOUND OF ORGAN MUSIC.");
	if (IRIT == 24018)
		printf("\nYOU CAN HEAR A FAUCET DRIPPING.");
	if (IRIT == 24019)
		printf("\nYOU CAN HEAR THE SOUND OF YOUR HEARTBEAT.");
	if ((IRIT == 24020) || (IRIT == 24024) || (IRIT == 24025))
		printf("\nYOU CAN HEAR THE WIND BLOWING OUTSIDE THE WINDOW "
			"TO THE ");
	if (IRIT == 24020) for (L=0; L<8; L++) putchar(IDTN[IDS][L]);
	if (IRIT == 24024) for (L=0; L<8; L++) putchar(IDTN[IDE][L]);
	if (IRIT == 24025) for (L=0; L<8; L++) putchar(IDTN[IDN][L]);
	if (IRIT == 24021)	
		printf("\nYOU CAN HEAR THE WIND WHISTLING BETWEEN YOUR EARS.");
	if ((IRIT == 24023) || (IRIT == 24026))
		printf("\nYOU CAN HEAR THE SOUNDS OF CHAINS RATTLING ABOVE "
			"YOU.");
	if (IRIT == 24027)
		printf("\nYOU CAN HEAR YOURSELF BREATHING.");
	if (IRIT == 24028)
		printf("\nYOU CAN HEAR SIX DISTINCT ECHOES FOR EACH SOUND "
			"YOU MAKE.");
	if (IRIT == 24029)
		printf("\nYOU CAN HEAR FIVE DISTINCT ECHOES FOR EACH SOUND "
			"YOU MAKE.");
	if (IRIT == 24030)
		printf("\nYOU CAN HEAR FOUR DISTINCT ECHOES FOR EACH SOUND "
			"YOU MAKE.");
	if (IRIT == 24031)
		printf("\nYOU CAN HEAR THREE DISTINCT ECHOES FOR EACH SOUND "
			"YOU MAKE.");
	if (IRIT == 24032)
		printf("\nYOU CAN HEAR TWO DISTINCT SOUNDS FOR EACH SOUND "
			"YOU MAKE.");
	if ((IRIT == 24033) || (IRIT == 24034))
		printf("\nYOU CAN HEAR AN ECHO FOR EACH SOUND YOU MAKE.");
	if ((IRIT > 24035) && (IRIT < 24041))
		printf("\nYOU CAN HEAR THE SOUND OF THE WALL CLOSING.");
	if (IRIT == 24041)
		printf("\nYOU CAN HEAR WATER FLOWING BELOW YOU.");
	if ((IRIT > 24041) && (IRIT < 24054) && (IRES[4] != 0))
		printf("\nYOU CAN HEAR SOMETHING DIGGING NEARBY.");
	if ((IRIT > 24041) && (IRIT < 24054) && (IRES[4] == 0))
		printf("\nYOU CAN HEAR CREAKING LIKE THE TUNNEL IS GOING TO "
			"CAVE IN.");
	if (IRIT == 24088)
		printf("\nYOU CAN HEAR THE FAINT SOUND OF MACHINERY.");
	if (IRIT == 24100)
		printf("\nYOU CAN HEAR NOTHING OF INTEREST.");
	if (IRIT == 24105)
		printf("  ");
	if (IPR[3] == 24108)
		printf("\nYOU CAN HEAR A CLOCK TICKING.");
	if (IPR[3] == 24110)
		printf("\nYOU CAN HEAR SOMETHING MAKING NOISE NEARBY.");
	if (IPR[3] == 24120)
		printf("\nYOU CAN HEAR SOMEONE MAKING NOISE NEARBY.");
	if (IPR[3] == 24130)
		printf("\nYOU CAN HEAR SOMEONE WHISPERING NEARBY.");
	if (IPR[3] == 24140)
		printf("\nYOU CAN HEAR SOMEONE TALKING NEARBY.");
	if (IPR[3] == 24150)
		printf("\nYOU CAN HEAR THE WOODSMAN AND HUNTER LAUGHING AND "
			"SAYING SOMETHING\nABOUT CLOSING THE GATE.");
	if (IPR[3] == 24160) {
		printf("\nYOU CAN HEAR THE MASTER AND THE BUTLER SAYING "
			"SOMETHING ABOUT THE ");
		for (L=0; L<8; L++) putchar(IRSN[ITST[9]][L]);
		printf(" AND MURDER.");
	}
	if (IPR[3] == 24162) {
		printf("\nYOU CAN HEAR THE MASTER AND THE BUTLER SAYING "
			"SOMETHING ABOUT THE\nSCENE OF THE MURDER BEING "
			"THE ");
		for (L=0; L<16; L++) putchar(IRNM[IFSD(IROM[ITST[8]])][L]);
	}
	if (IPR[3] == 24164) 
		printf("\nYOU HEAR THE MASTER TELLING THE BUTLER ABOUT A "
			"MURDER WITH THE %s", IWP);
	if (IPR[3] == 24170) {
		printf("\nYOU CAN HEAR THE COOK AND THE GARDENER SAYING "
			"SOMETHING ABOUT THE ");
		for (L=0; L<8; L++) putchar(IRSN[ITST[9]][L]);
		printf("\nAND MURDER.");
	}
	if (IPR[3] == 24172) {
		printf("\nYOU CAN HEAR THE COOK AND THE GARDENER SAYING "
			"SOMETHING ABOUT THE\nSCENE OF THE MURDER BEING "
			"THE ");
		for (L=0; L<16; L++) putchar(IRNM[IFSD(IROM[ITST[8]])][L]);
	}
	if (IPR[3] == 24174) 
		printf("\nYOU HEAR THE COOK TELLING THE GARDENER ABOUT A "
			"MURDER WITH THE %s", IWP);
	if (IPR[3] == 24180) {
		printf("\nYOU CAN HEAR THE LADY AND THE MAID SAYING SOMETHING "
			"ABOUT THE ");
		for (L=0; L<8; L++) putchar(IRSN[ITST[9]][L]);
		printf("\nAND MURDER.");
	}
	if (IPR[3] == 24182) {
		printf("\nYOU CAN HEAR THE LADY AND THE MAID SAYING "
			"SOMETHING ABOUT THE\nSCENE OF THE MURDER BEING "
			"THE ");
		for (L=0; L<16; L++) putchar(IRNM[IFSD(IROM[ITST[8]])][L]);
		printf("\nAND MURDER.");
	}
	if (IPR[3] == 24184) 
		printf("\nYOU HEAR THE LADY TELLING THE MAID ABOUT A "
			"MURDER WITH THE %s", IWP);
	IPR[2] = 0;
	IPR[3] = 0;
	if (IRIT == 12040) {
		IWRD[0][9] = 25;
		goto MMSC;
	}
	goto MMSB;
	

MMSJ:
	IR=ITST[1];
	R=RND;
	switch (IWRD[0][9] - 53) {
		case 1: goto MMSJ_1000;
		case 2: goto MMSJ_1000;
		case 3: goto MMSJ_2000;
		case 4: goto MMSJ_3000;
		case 5: goto MMSJ_4000;
		case 6: goto MMSJ_5000;
		case 7: goto MMSJ_6000;
		case 8: goto MMSJ_7000;
	}
MMSJ_1000:
	if (IWRD[0][10] == 1) {
		IPR[2] = 1000;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((IWRD[0][10] == 1) || (IWRD[1][8] != 3)) {
		IPR[2] = 1000;
		IPR[3] = 0;
		goto MMSK;
	}
	if (IWRD[1][9] != 92) goto MMSJ_15100;
	if (((IR > 27) && (IR != 36) && (IR != 54) && (IR != 91) &&
	     (IR != 90)) || (IR == 3) || (IR == 4) || (IR == 13) ||
	     (IR == 16) || (IR == 21) || (IR == 22)) {
		IPR[2] = 2030;
		IPR[3] = 0;
		goto MMSK;
	}
	for (J=1; J<=240; J++) {
		if (IFSD(IXT[J]) != IR) continue;
		if (((J-1)/60+1) != (ITST[4]/1000)) continue;
		IC=ITFD(IXT[J]);
		if ((IC == 4) || (IC == 13) || (IC == 22) || (J == 62) ||
		    (J == 70) || (J == 133) || (J == 182) || (J == 190)) {
			IPR[2] = 15030;
			IPR[3] = 0;
			goto MMSK;
		}
		if (IXT[J] < 10000) {
			IPR[2] = 15020;
			IPR[3] = 0;
			goto MMSK;
		}
		if (((IXT[J] < 20000) && (IWRD[0][9] == 54)) || 
		    ((IXT[J] > 20000) && (IWRD[0][9] == 55))) {
			IPR[2] = 8020;
			IPR[3] = 0;
			goto MMSK;
		}
		if ((IVEN[46] < 20000) && (J == 64)) {
			IPR[2] = 15041;
			IPR[3] = 0;
			goto MMSK;
		}
		if ((IVEN[2] < 20000) && (J != 64)) {
			IPR[2] = 15040;
			IPR[3] = 0;
			goto MMSK;
		}
		if (IWRD[0][9] == 54) IXT[J] = IXT[J] - 10000;
		if (IWRD[0][9] == 55) IXT[J] = IXT[J] + 10000;
		if ((J < 121) && (IWRD[0][9] == 54)) 	
			IXT[J+120] = IXT[J+120] - 10000;
		if ((J > 120) && (IWRD[0][9] == 54)) 
			IXT[J-120] = IXT[J-120] - 10000;
		if ((J < 121) && (IWRD[0][9] == 55))
			IXT[J+120] = IXT[J+120] + 10000;
		if ((J > 120) && (IWRD[0][9] == 55)) 
			IXT[J-120] = IXT[J-120] + 10000;
		IPR[2] = 1040;
		IPR[3] = 0;
		goto MMSK;
	}
	IPR[2] = 15030;
	IPR[3] = 0;
	goto MMSK;
MMSJ_15100:
	if ((IWRD[1][9] != 54) && (IWRD[1][9] != 35)) goto MMSJ_15200;
	if ((IVEN[35] < 20000) && (IVEN[35] != (600+IR))) {
		IPR[2] = 8030;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((IWRD[0][9] == 55) && (IFD(IVEN[35]) == 0)) {
		IPR[2] = 8020;
		IPR[3] = 0;
		goto MMSK;
	}
	if (IWRD[0][9] == 55) {
		IPR[2] = 2010;
		IPR[3] = 0;
		goto MMSK;
	}
	if (IVEN[2] < 20000) {
		IPR[2] = 15040;
		IPR[3] = 0;
		goto MMSK;
	}
	IPR[2] = 15110;
	IPR[3] = 0;
	goto MMSK;
MMSJ_15200:
	IPR[2] = 8010;
	IPR[3] = 0;
	goto MMSK;
MMSJ_2000:
	if (IWRD[0][10] == 1) {
		IPR[2] = 1000;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((IWRD[0][10] != 2) || ((IWRD[1][8] != 4) && (IWRD[1][8] != 3)) ||
	    ((IWRD[1][8] == 3) && (IWRD[1][9] != 54))) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSK;
	}
	if (IVEN[9] < 20000) {
		IPR[2] = 16010;
		IPR[3] = 0;
		goto MMSK;
	}
	MMRLret=3;
	goto MMRL;
RLret3:
	if ((IFSD(IRES[IWRD[1][9]]) != IR) && (IWRD[1][8] != 3)) {
		IPR[2] = 2030;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((IWRD[1][8] == 3) && (IFD(IVEN[35]) != 0)) {
		IPR[2] = 2030;
		IPR[3] = 0;
		goto MMSK;
	}
	IC=0;
	if (IVEN[20] > 20000) IC=1;
	if (IWRD[1][9] == 10) goto MMSJ_16020;
	if ((ITST[11] == 0) && (IVEN[20] < 20000)) {
		IPR[2] = 16020;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((ITST[11] == 0) && (IWRD[1][9] == 10) && (IVEN[20] <  20000)) 
		IVEN[20] = 0;
	if (((ITST[11] > 0) && (IWRD[1][9] != 10)) || ((IWRD[1][9] == 10) &&
	     (IVEN[20] < 20000)))
		ITST[11] -= 1;
	R=RN(R);
	if (R < 0.2) {
		IPR[2] = 16030;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((R < 0.4) && (IRES[IWRD[1][9]]/1000 == 0)) {
		IRES[IWRD[1][9]] = IRES[IWRD[1][9]] + 5000;
		IPR[2] = 16031;
		IPR[3] = 0;
		goto MMSK;
	}
	if (IWRD[1][8] == 3) {
		IPR[2] = 16035;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((IWRD[1][9] == 1) || (IWRD[1][9] == 5) || 
	    ((IWRD[1][9] == 10) && (IC == 0)) ||
	    (IRES[IWRD[1][9]] > 10000)) {
		IPR[2] = 16040;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((IWRD[1][9] == 13) && (IR != 33)) IRES[13] = 533;
	if ((IWRD[1][9] == 13) && (IR == 33)) IRES[13] = 534;
	if (IWRD[1][9] == 13) {
		IPR[2] = 16045;
		IPR[3] = 0;
		goto MMSK;
	}
MMSJ_16020:
	if (IWRD[1][9] == 10) IRES[ITST[39]] = 10000+IR;
	if (IWRD[1][9] == 10) ITST[2] = ITST[2]+75;
	if ((IWRD[1][9] == 16) && (R < 0.8) && (IVEN[45] == 14416) &&
	    (IRES[16]/1000 < 4)) {
		IPR[2] = 16046;
		IPR[3] = 0;
		goto MMSK;
	}
	IRES[IWRD[1][9]] = 10000+IR;
	if (IWRD[1][9] == 10) IRES[10] = 0;
	IPR[2] = 16050;
	IPR[3] = 0;
	goto MMSK;
MMSJ_3000:
	if ((IWRD[0][10] != 2) || (IWRD[1][8] != 3)) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSK;
	}
	MMRLret=4;
	goto MMRL;
RLret4:
	if (IVEN[33] < 20000) {
		IPR[2] = 20010;
		IPR[3] = 0;
		goto MMSK;
	}
	if (IVEN[33] > 22000) {
		IPR[2] = 20020;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((IWRD[1][9] < 54) && (IVEN[IWRD[1][9]] < 20000)) {
		IPR[2] = 3010;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((IWRD[1][9] > 47) && (IFSD(IVEN[IWRD[1][9]]) != IR) &&
	    (IFSD(IVEN[IWRD[1][9]]) != 0)) {
		IPR[2] = 2030;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((IWRD[1][9] > 45) && (IWRD[1][9] < 51)) {
		IPR[2] = 20030;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((IWRD[1][9] == 91) && (IROM[IR] < 10000)) {
		IPR[2] = 2030;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((IWRD[1][9] == 92) || (IWRD[1][9] == 93)) {
		IPR[2] = 8020;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((IWRD[1][9] == 74) && (IR == 85)) IXT[322] = 9185;
	if ((IWRD[1][9] == 74) && (IR == 86)) IXT[322] = 8586;
	IVEN[74] = IVEN[74] + 1000;
	IPR[2] = 1040;
	IPR[3] = 0;
	goto MMSK;
MMSJ_4000:
	IPR[2] = 320;
	IPR[3] = 0;
	goto MMSK;
MMSJ_5000:
	if ((IWRD[0][10] != 2) || (IWRD[1][8] != 3) || (IWRD[1][9] != 54)) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSK;
	}
	MMRLret=5;
	goto MMRL;
RLret5:
	if (IFD(IVEN[35]) == 1) {
		IPR[2] = 8020;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((IFSD(IVEN[35]) != 1) || (IVEN[35] > 10000)) {
		IPR[2] = 22010;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((IVEN[11] < 20000) && (IVEN[12] < 20000) && (IVEN[14] < 20000) &&
	    (IVEN[32] < 20000) && (IVEN[34] < 20000)) {
		IPR[2] = 22020;
		IPR[3] = 0;
		goto MMSK;
	}
	IPR[2] = 16035;
	IPR[3] = 0;
	goto MMSK;
MMSJ_6000:
	if ((IWRD[0][10] != 4) || (IWRD[1][8] != 3) || (IWRD[2][8] != 5) ||
	    (IWRD[3][8] != 3)) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((IWRD[1][9] != 17) || (IWRD[2][9] != 1) || (IWRD[3][9] != 66)) 
		goto MMSJ_6100;
	if (IVEN[17] < 20000) {
		IPR[2] = 3010;
		IPR[3] = 0;
		goto MMSK;
	}
	if (IR != 41) {
		IPR[2] = 6010;
		IPR[3] = 0;
		goto MMSK;
	}
	IVEN[17] = 1141;
	IPR[2] = 6020;
	IPR[3] = 0;
	goto MMSK;
MMSJ_6100:
	if ((IWRD[1][9] != 46) || (IWRD[2][9] != 2) || (IWRD[3][9] != 85)) 
		goto MMSJ_6200;
	if (IVEN[46] < 20000) {
		IPR[2] = 3010;
		IPR[3] = 0;
		goto MMSK;
	}
	if (IR != 1) {
		IPR[2] = 6010;
		IPR[3] = 0;
		goto MMSK;
	}
	IVEN[46] = 1101;
	IPR[2] = 1040;
	IPR[3] = 0;
	goto MMSK;
MMSJ_6200:
	if ((IWRD[1][9] != 5) || (IWRD[2][9] != 2) || (IWRD[3][9] != 77))
		goto MMSJ_6300;
	if (IVEN[5] < 20000) {
		IPR[2] = 3010;
		IPR[3] = 0;
		goto MMSK;
	}
	if (IR != 16) {
		IPR[2] = 6010;
		IPR[3] = 0;
		goto MMSK;
	}
	IVEN[5] = 1316;
	IPR[2] = 1040;
	IPR[3] = 0;
	goto MMSK;
MMSJ_6300:
	if ((IWRD[1][9] != 4) || (IWRD[2][9] != 2) || (IWRD[3][9] != 85))
		goto MMSJ_6900;
	if (IVEN[4] < 20000) {
		IPR[2] = 3010;
		IPR[3] = 0;
		goto MMSK;
	}
	if (IR != 93) {
		IPR[2] = 6010;
		IPR[3] = 0;
		goto MMSK;
	}
	if (IFD(IVEN[4]) == 1) IVEN[4]=1493;
	if (IFD(IVEN[4]) != 1) IVEN[4]=493;
	IPR[2] = 1040;
	IPR[3] = 0;
	goto MMSK;
MMSJ_6900:
	IPR[2] = 320;
	IPR[3] = 0;
	goto MMSK;
MMSJ_7000:
	if (IWRD[0][10] == 1) goto MMSJ_7500;
	if ((IWRD[0][10] != 3) || (IWRD[1][8] != 5) || (IWRD[1][9] != 13) ||
	    (IWRD[2][8] != 4)) {
		IPR[2] = 320;
		IPR[3] = 0;
		goto MMSK;
	}
	if (IFSD(IRES[IWRD[2][9]]) != IR) {
		IPR[2] = 8031;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((IWRD[2][9] == 1) || (IWRD[2][9] == 15) || (IWRD[2][9] == 4)) {
		IPR[2] = 7010;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((IR != 1) && (IR != 20) && (IR != 24) && (IR != 25)) {
		IPR[2] = 7020;
		IPR[3] = 0;
		goto MMSK;
	}
	if ((ITST[32] != 1) || ((IR == 1) && (IXT[61] < 20000)) ||
	    ((IR == 20) && ((IXT[12] < 20000) || (IROM[20] > 20000))) ||
	    ((IR == 25) && ((IXT[73] < 20000) || (IROM[25] > 20000))) ||
	    (IR == 24)) {
		IPR[2] = 7030;
		IPR[3] = 0;
		goto MMSK;
	}
	for (I=1; I<=53; I++) {
		if (IVEN[I] >= 20000) {
			IPR[2] = 7030;
			IPR[3] = 0;
			goto MMSK;
		}
	}
	for (I=1; I<=17; I++) {
		if ((IFSD(IRES[I]) == IR) && (IWRD[2][9] != I)) {
			IPR[2] = 7030;
			IPR[3] = 0;
			goto MMSK;
		}
	}
	if ((IWRD[2][9] == 3) || (IWRD[2][9] == 7) || (IWRD[2][9] == 14))
		ITST[32]=2;
	if ((IWRD[2][9] == 2) || (IWRD[2][9] == 8) || (IWRD[2][9] == 9))
		ITST[32]=3;
	ITST[5] = ITST[5]+25;
	IPR[2] = 7040;
	IPR[3] = 0;
	goto MMSK;
MMSJ_7500:
	if ((IR != 1) && (IR != 10) && (IR != 20) && (IR != 24) && (IR != 25)) {
		IPR[2] = 7510;
		IPR[3] = 0;
		goto MMSK;
	}
	for (I=2; I<=17; I++) {
		if (IFSD(IRES[I]) == I) {
			IPR[2] = 7530;
			IPR[3] = 0;
			goto MMSK;
		}
	}
	ITST[5] = ITST[5]+25;
	IPR[2] = 7540;
	IPR[3] = 0;
	goto MMSK;

MMSK:
	IRIT=IPR[2];
	IW63=IWRD[2][9];
	IFC=ITST[4]/1000;
	IDN=1;
	IDE=2;
	IDS=3;
	IDW=4;
	if (IVEN[6] < 20000) IDN=10-IFC;
	if (IDN == 6) IDN=10;
	if (IVEN[6] < 20000) IDE=11-IFC;
	if (IVEN[6] < 20000) IDS=12-IFC;
	if (IDS == 11) IDS=7;
	if (IVEN[6] < 20000) IDW=13-IFC;
	if (IDW > 10) IDW=IDW-4;
	if (IRIT == 320) 
		printf("\nI CANNOT FIGURE OUT WHAT YOU ARE TRYING TO SAY");
	if (IRIT == 520)
		printf("\nNOTHING HAPPENS");
	if (IRIT == 1000) {
		printf("\nI AM NOT A MIND READER, YOU WILL HAVE TO TELL ME "
			"WHAT TO ");
		for (L=0; L<8; L++) putchar(IWRD[0][L]);
	}
	if (IRIT == 1040) printf("\nOKAY");
	if (IRIT == 1065) printf("\nPROGRAM ERROR AT LINE %d", IPR[3]);
	if ((IPR[3] == 7) || (IPR[3] == 16) || (IPR[3] == 25)) IC=IDS;	
	if ((IPR[3] == 5) || (IPR[3] == 14) || (IPR[3] == 23)) IC=IDW;	
	if ((IPR[3] == 1) || (IPR[3] == 10) || (IPR[3] == 19)) IC=IDN;	
	if (IRIT == 1095) {
		printf("\nYOUR ACTION OPENED A PANEL INTO THE SECRET PASSAGE "
			"THAT YOU CAN\nENTER BY GOING ");
		for (I=0; I<8; I++) putchar(IDTN[IC][I]);	
	}
	if (IRIT == 2010) {
		printf("\nYOU CANNOT ");
		for (L=0; L<4; L++) putchar(IWRD[0][L]);
		printf(" THE ");
		for (L=0; L<8; L++) putchar(IWRD[1][L]);
	}
	if (IRIT == 2020) {
		printf("\nYOU ALREADY HAVE THE ");
		for (L=0; L<8; L++) putchar(IWRD[1][L]);
	}
	if (IRIT == 2025) {
		printf("\nIT IS NOT IN THE POSSESSION OF THE ");
		for (L=0; L<8; L++) putchar(IWRD[3][L]);
	}
	if (IRIT == 2030) {
		printf("\nI DON'T SEE THE ");
		for (L=0; L<8; L++) putchar(IWRD[1][L]);
	}
	if (IRIT == 3010) {
		printf("\nYOU DO NOT HAVE THE ");
		for (L=0; L<8; L++) putchar(IWRD[1][L]);
	}
	if (IRIT == 3110) {
		printf("\nYOU NO LONGER HAVE THE ");
		for (L=0; L<8; L++) putchar(IWRD[1][L]);
	}
	if (IRIT == 4010) {
		printf("\nFIRST YOU HAVE TO FIND THE ");
		for (L=0; L<8; L++) putchar(IWRD[3][L]);
	}
	if (IRIT == 4050) printf("\nTHANKS. IT NEEDS THAT AROUND HERE.");
	if (IRIT == 5020) {
		printf("\nI DO NOT SEE THE ");
		for (L=0; L<8; L++) putchar(IWRD[2][L]);
	}
	if (IRIT == 6010) printf("\nYOU MUST NOT BE PAYING ATTENTION.");
	if (IRIT == 6020) printf("\nTHE FLOOR UNDER YOU MOVED SLIGHTLY AS IF "
		"THE AMULET ACTIVATED SOMETHING.");
	if (IRIT == 7010) printf("\nYOU MUST BE SOME KIND OF PERVERT.");
	if (IRIT == 7020) {
		printf("\n'THIS IS NOT THE TIME OR PLACE.' SAYS THE ");
		for (L=0; L<8; L++) putchar(IWRD[2][L]);
	}
	if (IRIT == 7030) {
		printf("\n'NOT NOW. SOMETHING IS NOT RIGHT.' SAYS THE ");
		for (L=0; L<8; L++) putchar(IWRD[2][L]);
	}
	if (IRIT == 7040) {
		printf("\nYOU FEEL YOURSELF GETTING AROUSED AS YOU WATCH "
			"A SEXY STRIP BY THE ");
		for (L=0; L<8; L++) putchar(IWRD[2][L]);
		printf("\nBEFORE PULLING YOU INTO BED. EAGERLY, YOU CARESS "
			"EACH OTHER AND .........\n\nAN HOUR LATER, AFTER "
			"MANY SEXUAL DELIGHTS AND A NAP; YOU SEE THE ");
		for (L=0; L<8; L++) putchar(IWRD[2][L]);
		printf("\nFINISH DRESSING, GIVING YOU A THANKFUL SMILE AS "
			"YOU WAKE UP.");
	}
	if (IRIT == 7050) printf("\nNOTHING");
	if (IRIT == 7510) printf("\nYOU CANNOT GET COMFORTABLE HERE.");
	if (IRIT == 7530) printf("\nSOMEONE IN THE ROOM IS KEEPING YOU AWAKE.");
	if (IRIT == 7540) printf("\nYOU SLEEP FOR AN HOUR AND WAKE-UP "
		"REFRESHED.");
	if (IRIT == 8010) printf("\nI DON'T KNOW WHAT YOU EXPECT TO HAPPEN");
	if (IRIT == 8015) {
		printf("\nYOUR FOOD HAS BEEN EATEN BY THE ");
		for (L=0; L<8; L++) putchar(IWRD[2][L]);
	}
	if (IRIT == 8020) printf("\nIT ALREADY IS");
	if (IRIT == 8030) {
		printf("\nI DON'T SEE THE ");
		for (L=0; L<8; L++) putchar(IWRD[1][L]);
	}
	if (IRIT == 8031) {
		printf("\nI DON'T SEE THE ");
		for (L=0; L<8; L++) putchar(IWRD[2][L]);
	}
	if (IRIT == 15030) printf("\nYOU ARE NOT FACING A DOOR");
	if (IRIT == 15020) printf("\nTHE DOOR YOU ARE FACING HAS NO LOCK");
	if (IRIT == 15040) printf("\nYOU NEED A DOOR KEY");
	if (IRIT == 15041) printf("\nYOU NEED A DUNGEON KEY");
	if (IRIT == 15110) printf("\nYOUR KEY WILL NOT FIT THE PADLOCK");
	if (IRIT == 16010) printf("\nYOU NEED A GUN TO SHOOT ANYONE");
	if (IRIT == 16020) printf("\nYOUR GUN IS OUT OF BULLETS");
	if (IRIT == 16030) printf("\nYOU MISSED");
	if (IRIT == 16031) {
		printf("\nYOU WOUNDED THE ");
		for (L=0; L<8; L++) putchar(IWRD[1][L]);
	}
	if (IRIT == 16035) printf("\nYOU JUST SMASHED THE PADLOCK TO PIECES");
	if (IRIT == 16035) IVEN[35] += 1000;
	if (IRIT == 16040) {
		printf("\nYOU JUST WASTED A SHOT. IT DID YOU NO GOOD TO SHOOT "
			"THE ");
		for (L=0; L<8; L++) putchar(IWRD[1][L]);
	}
	if (IRIT == 16045) printf("\nYOU KILLED THE DWARF AND HE DISAPPEARED "
		"IN A CLOUD OF GREASY BLACK SMOKE");
	if (IRIT == 16046) printf("\nTHE BULLET BOUNCED OFF THE WARRIOR'S "
		"SHIELD.");
	if (IRIT == 16045) IRES[13] += 100;
	if (IRIT == 16050) {
		printf("\nYOU JUST SHOT AND KILLED THE ");
		for (L=0; L<8; L++) putchar(IWRD[1][L]);
	}
	if (IRIT == 20010) printf("\nYOU NEED AN OILCAN OR SOMETHING");
	if (IRIT == 20020) printf("\nYOUR OILCAN IS EMPTY");
	if (IRIT == 20030) printf("\nDON'T BE SILLY");
	if (IRIT == 21020) printf("\nLU RECORDING NOT SUPPORTED");
	if (IRIT == 22010) printf("\nTHE CHEST IS NOT WHERE YOU CAN BREAK "
		"THE PADLOCK");
	if (IRIT == 22020) printf("\nYOU DO NOT HAVE ANYTHING THAT WILL "
		"BREAK THE PADLOCK");
	IPR[2]=IPR[3]=0;
	goto MMSB;

/*
 * Light-handling routine
 */
MMRL:
	/*
	 * If it's sunlit here or there's a light source or a the lantern,
	 * candle or torch is on/lit and here with a player or inhabitant...
	 */
	IR=ITST[1];
	for (I=1; I<=3; I++) {
		if (I == 1) K=4;
		if (I == 2) K=19;
		if (I == 3) K=27;
		if ((IFD(IVEN[K]) == 1) && 
		    (((IVEN[K] < 10000) && (IFSD(IVEN[K]) == IR)) || 
		     (IVEN[K] > 20000) || ((IVEN[K] > 10000) && 
		     (IVEN[K] < 20000) && (IFSD(IRES[IFSD(IVEN[K])]) == IR))))
			goto MMRL_return;
	}
	
	/* lamp */
	if ((IR == 10) && (IVEN[76] > 1000)) goto MMRL_return;
	/* fireplace */
	if ((IR == 14) && (IVEN[89] > 1000)) goto MMRL_return;
	/* match */
	if (IVEN[31] > 21000) goto MMRL_return;
	/* outside and daytime */
	if ((IR > 53) && (ITST[5] < 300)) goto MMRL_return;
	if (IR > 53) {
		IPR[2]=1150;
		IPR[3]=1;
		goto MMSD;
	}
	/* open curtain */
	if ((IROM[IR] > 20000) && (ITST[5] < 300)) goto MMRL_return;
	if (IROM[IR] > 20000) {	
		IPR[2]=1150;
		IPR[3]=1;
		goto MMSD;
	}
	if ((ITST[5] < 300) && (IROM[IR] > 10000)) {
		IPR[2]=1150;
		IPR[3]=2;
		goto MMSD;
	}
	IPR[2]=1150;
	IPR[3]=0;
	goto MMSD;

	/*
	 * This is an evil kludge to make the raw port work.  MMRL was
	 * originally a subroutine, but had access to the global variables
 	 * in the Fortran IV common blocks.  We maintain a single flat scope
	 * by having no real subroutines/functions here so global variables
	 * aren't necessary, but that means keeping track of where 
	 * to "return" to in the main code.  This should be split out
	 * and the vars should be passed in as parameters instead of doing
	 * it this way.
	 */
MMRL_return:
	switch (MMRLret) {
		case 0: goto RLret0;
		case 1: goto RLret1;
		case 2: goto RLret2;
		case 3: goto RLret3;
		case 4: goto RLret4;
		case 5: goto RLret5;
		case 6: goto RLret6;
		case 7: goto RLret7;
		case 8: goto RLret8;
		case 9: goto RLret9;
		default: printf("ack! unknown rl-ret: %d!!\n  This is a bug, "
			"send it to garnett@catbelly.com!\n", MMRLret);
			printf("Sorry, gotta die now, I don't know what to do");
			pak();
			exit(0);
	}

	return 0;
}
