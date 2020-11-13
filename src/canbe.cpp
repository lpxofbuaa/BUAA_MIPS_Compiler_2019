#include"canbe.h"
#include "main.h"
#include "is.h"

using namespace std;

bool canbeChar() {
	char tmp;
	tmp = in.getc();
	if (tmp == 'a') {
		token += tmp;
		tmp = in.getc();
		if (tmp == 'r') {
			token += tmp;
			tmp = in.getc();
			if ((not isAlpha(tmp)) && (not isDigital(tmp))) {
				pick = tmp;
				return true;
			}
		}
	}
	pick = tmp;
	return false;
}

bool canbeConst() {
	char tmp;
	tmp = in.getc();
	if (tmp == 'n') {
		token += tmp;
		tmp = in.getc();
		if (tmp == 's') {
			token += tmp;
			tmp = in.getc();
			if (tmp == 't') {
				token += tmp;
				tmp = in.getc();
				if ((not isAlpha(tmp)) && (not isDigital(tmp))) {
					pick = tmp;
					return true;
				}
			}
		}
	}
	pick = tmp;
	return false;
}

bool canbeInt() {
	char tmp;
	tmp = in.getc();
	if (tmp == 't') {
		token += tmp;
		tmp = in.getc();
		if ((not isAlpha(tmp)) && (not isDigital(tmp))) {
			pick = tmp;
			return true;
		}
	}
	pick = tmp;
	return false;
}

bool canbeIf() {
	char tmp;
	tmp = in.getc();
	if ((not isAlpha(tmp)) && (not isDigital(tmp))) {
		pick = tmp;
		return true;
	}
	pick = tmp;
	return false;
}

bool canbeVoid() {
	char tmp;
	tmp = in.getc();
	if (tmp == 'o') {
		token += tmp;
		tmp = in.getc();
		if (tmp == 'i') {
			token += tmp;
			tmp = in.getc();
			if (tmp == 'd') {
				token += tmp;
				tmp = in.getc();
				if ((not isAlpha(tmp)) && (not isDigital(tmp))) {
					pick = tmp;
					return true;
				}
			}
		}
	}
	pick = tmp;
	return false;
}

bool canbeMain() {
	char tmp;
	tmp = in.getc();
	if (tmp == 'a') {
		token += tmp;
		tmp = in.getc();
		if (tmp == 'i') {
			token += tmp;
			tmp = in.getc();
			if (tmp == 'n') {
				token += tmp;
				tmp = in.getc();
				if ((not isAlpha(tmp)) && (not isDigital(tmp))) {
					pick = tmp;
					return true;
				}
			}
		}
	}
	pick = tmp;
	return false;
}

bool canbeElse() {
	char tmp;
	tmp = in.getc();
	if (tmp == 'l') {
		token += tmp;
		tmp = in.getc();
		if (tmp == 's') {
			token += tmp;
			tmp = in.getc();
			if (tmp == 'e') {
				token += tmp;
				tmp = in.getc();
				if ((not isAlpha(tmp)) && (not isDigital(tmp))) {
					pick = tmp;
					return true;
				}
			}
		}
	}
	pick = tmp;
	return false;
}

bool canbeDo() {
	char tmp;
	tmp = in.getc();
	if (tmp == 'o') {
		token += tmp;
		tmp = in.getc();
		if ((not isAlpha(tmp)) && (not isDigital(tmp))) {
			pick = tmp;
			return true;
		}
	}
	pick = tmp;
	return false;
}

bool canbeWhile() {
	char tmp;
	tmp = in.getc();
	if (tmp == 'h') {
		token += tmp;
		tmp = in.getc();
		if (tmp == 'i') {
			token += tmp;
			tmp = in.getc();
			if (tmp == 'l') {
				token += tmp;
				tmp = in.getc();
				if (tmp == 'e') {
					token += tmp;
					tmp = in.getc();
					if ((not isAlpha(tmp)) && (not isDigital(tmp))) {
						pick = tmp;
						return true;
					}
				}
			}
		}
	}
	pick = tmp;
	return false;
}

bool canbeFor() {
	char tmp;
	tmp = in.getc();
	if (tmp == 'o') {
		token += tmp;
		tmp = in.getc();
		if (tmp == 'r') {
			token += tmp;
			tmp = in.getc();
			if ((not isAlpha(tmp)) && (not isDigital(tmp))) {
				pick = tmp;
				return true;
			}
		}
	}
	pick = tmp;
	return false;
}

bool canbeScanf() {
	char tmp;
	tmp = in.getc();
	if (tmp == 'c') {
		token += tmp;
		tmp = in.getc();
		if (tmp == 'a') {
			token += tmp;
			tmp = in.getc();
			if (tmp == 'n') {
				token += tmp;
				tmp = in.getc();
				if (tmp == 'f') {
					token += tmp;
					tmp = in.getc();
					if ((not isAlpha(tmp)) && (not isDigital(tmp))) {
						pick = tmp;
						return true;
					}
				}
			}
		}
	}
	pick = tmp;
	return false;
}

bool canbePrintf() {
	char tmp;
	tmp = in.getc();
	if (tmp == 'r') {
		token += tmp;
		tmp = in.getc();
		if (tmp == 'i') {
			token += tmp;
			tmp = in.getc();
			if (tmp == 'n') {
				token += tmp;
				tmp = in.getc();
				if (tmp == 't') {
					token += tmp;
					tmp = in.getc();
					if (tmp == 'f') {
						token += tmp;
						tmp = in.getc();
						if ((not isAlpha(tmp)) && (not isDigital(tmp))) {
							pick = tmp;
							return true;
						}
					}
				}
			}
		}
	}
	pick = tmp;
	return false;
}

bool canbeReturn() {
	char tmp;
	tmp = in.getc();
	if (tmp == 'e') {
		token += tmp;
		tmp = in.getc();
		if (tmp == 't') {
			token += tmp;
			tmp = in.getc();
			if (tmp == 'u') {
				token += tmp;
				tmp = in.getc();
				if (tmp == 'r') {
					token += tmp;
					tmp = in.getc();
					if (tmp == 'n') {
						token += tmp;
						tmp = in.getc();
						if ((not isAlpha(tmp)) && (not isDigital(tmp))) {
							pick = tmp;
							return true;
						}
					}
				}
			}
		}
	}
	pick = tmp;
	return false;
}