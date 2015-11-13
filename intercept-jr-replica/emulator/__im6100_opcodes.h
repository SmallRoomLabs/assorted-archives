case 07000: /****** nop ******/
    CYCLES(10);
    break;

case 07001: /****** iac ******/
    AC = (AC + 1) & 07777;
    if (AC == 0) L ^= 1;
    CYCLES(10);
    break;

case 07002: /****** bsw ******/
    AC = ((AC >> 6) | (AC << 6)) & 07777;
    CYCLES(15);
    break;

case 07004: /****** ral ******/
    temp8 = AC >> 11;
    AC = ((AC << 1) | L) & 07777;
    L = temp8;
    CYCLES(15);
    break;

case 07006: /****** rtl ******/
    temp8 = AC >> 11;
    AC = ((AC << 1) | L) & 07777;
    L = temp8;
    temp8 = AC >> 11;
    AC = ((AC << 1) | L) & 07777;
    L = temp8;
    CYCLES(15);
    break;

case 07010: /****** rar ******/
    temp8 = AC & 1;
    AC = (AC >> 1) | (L << 11);
    L = temp8;
    CYCLES(15);
    break;

case 07012: /****** rtr ******/
    temp8 = AC & 1;
    AC = (AC >> 1) | (L << 11);
    L = temp8;
    temp8 = AC & 1;
    AC = (AC >> 1) | (L << 11);
    L = temp8;
    CYCLES(15);
    break;

case 07020: /****** cml ******/
    L ^= 1;
    CYCLES(10);
    break;

case 07040: /****** cma ******/
    AC ^= 07777;
    CYCLES(10);
    break;

case 07041: /****** cia ******/
    AC ^= 07777;
    AC = (AC + 1) & 07777;
    if (AC == 0) L ^= 1;
    CYCLES(10);
    break;

case 07100: /****** cll ******/
    L = 0;
    CYCLES(10);
    break;

case 07104: /****** cll ral ******/
    L = 0;
    temp8 = AC >> 11;
    AC = ((AC << 1) | L) & 07777;
    L = temp8;
    CYCLES(15);
    break;

case 07106: /****** cll rtl ******/
    L = 0;
    temp8 = AC >> 11;
    AC = ((AC << 1) | L) & 07777;
    L = temp8;
    temp8 = AC >> 11;
    AC = ((AC << 1) | L) & 07777;
    L = temp8;
    CYCLES(15);
    break;

case 07110: /****** cll rar ******/
    L = 0;
    temp8 = AC & 1;
    AC = (AC >> 1) | (L << 11);
    L = temp8;
    CYCLES(15);
    break;

case 07112: /****** cll rtr ******/
    L = 0;
    temp8 = AC & 1;
    AC = (AC >> 1) | (L << 11);
    L = temp8;
    temp8 = AC & 1;
    AC = (AC >> 1) | (L << 11);
    L = temp8;
    CYCLES(15);
    break;

case 07120: /****** stl ******/
    L = 1;
    CYCLES(10);
    break;

case 07200: /****** cla ******/
    AC = 0;
    CYCLES(10);
    break;

case 07201: /****** cla iac ******/
    AC = 0;
    AC = (AC + 1) & 07777;
    if (AC == 0) L ^= 1;
    CYCLES(10);
    break;

case 07204: /****** gtl ******/
    AC = 0;
    temp8 = AC >> 11;
    AC = ((AC << 1) | L) & 07777;
    L = temp8;
    CYCLES(15);
    break;

case 07240: /****** sta ******/
    AC = 0;
    AC ^= 07777;
    CYCLES(10);
    break;

case 07300: /****** cla cll ******/
    AC = 0;
    L = 0;
    CYCLES(10);
    break;

case 07400: /****** nop ******/
    CYCLES(10);
    break;

case 07401: /****** nop ******/
    CYCLES(10);
    break;

case 07402: /****** hlt ******/
    HALT = 1;
    CYCLES(10);
    break;

case 07404: /****** osr ******/
    AC |= SWITCHREGISTER();
    CYCLES(15);
    break;

case 07410: /****** skp ******/
    SKIP(1);
    CYCLES(10);
    break;

case 07420: /****** snl ******/
    SKIP(L != 0);
    CYCLES(10);
    break;

case 07421: /****** mql ******/
    MQ = AC;
    AC = 0;
    CYCLES(10);
    break;

case 07430: /****** szl ******/
    SKIP(L == 0);
    CYCLES(10);
    break;

case 07440: /****** sza ******/
    SKIP(AC == 0);
    CYCLES(10);
    break;

case 07450: /****** sna ******/
    SKIP(AC != 0);
    CYCLES(10);
    break;

case 07460: /****** sza snl ******/
    SKIP((AC == 0) || (L != 0));
    CYCLES(10);
    break;

case 07470: /****** sna szl ******/
    SKIP((AC != 0) && (L == 0));
    CYCLES(10);
    break;

case 07500: /****** sma ******/
    SKIP(SIGN(AC) != 0);
    CYCLES(10);
    break;

case 07501: /****** mqa ******/
    AC |= MQ;
    CYCLES(10);
    break;

case 07510: /****** spa ******/
    SKIP(SIGN(AC) == 0);
    CYCLES(10);
    break;

case 07520: /****** sma snl ******/
    SKIP((SIGN(AC) != 0) || (L != 0));
    CYCLES(10);
    break;

case 07521: /****** swp ******/
    temp16 = AC;
    AC = MQ;
    MQ = temp16;
    CYCLES(10);
    break;

case 07530: /****** spa szl ******/
    SKIP((SIGN(AC) == 0) && (L == 0));
    CYCLES(10);
    break;

case 07540: /****** sma sza ******/
    SKIP((SIGN(AC) != 0) || (AC == 0));
    CYCLES(10);
    break;

case 07550: /****** spa sna ******/
    SKIP((SIGN(AC) == 0) && (AC != 0));
    CYCLES(10);
    break;

case 07560: /****** sma sza snl ******/
    SKIP((SIGN(AC) != 0) || (AC == 0) || (L != 0));
    CYCLES(10);
    break;

case 07570: /****** spa sna szl ******/
    SKIP((SIGN(AC) == 0) && (AC != 0) && (L == 0));
    CYCLES(10);
    break;

case 07600: /****** cla ******/
    AC = 0;
    CYCLES(10);
    break;

case 07601: /****** cla ******/
    AC = 0;
    CYCLES(10);
    break;

case 07604: /****** las ******/
    AC = 0;
    AC |= SWITCHREGISTER();
    CYCLES(15);
    break;

case 07621: /****** cam ******/
    AC = 0;
    MQ = AC;
    AC = 0;
    CYCLES(10);
    break;

case 07640: /****** sza cla ******/
    SKIP(AC == 0);
    AC = 0;
    CYCLES(10);
    break;

case 07650: /****** sna cla ******/
    SKIP(AC != 0);
    AC = 0;
    CYCLES(10);
    break;

case 07700: /****** sma cla ******/
    SKIP(SIGN(AC) != 0);
    AC = 0;
    CYCLES(10);
    break;

case 07701: /****** acm ******/
    AC = 0;
    AC |= MQ;
    CYCLES(10);
    break;

case 07710: /****** spa cla ******/
    SKIP(SIGN(AC) == 0);
    AC = 0;
    CYCLES(10);
    break;

case 07721: /****** cla swp ******/
    AC = 0;
    temp16 = AC;
    AC = MQ;
    MQ = temp16;
    CYCLES(10);
    break;

