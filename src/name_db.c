#include <stdlib.h>
#include <time.h>

static wchar_t player_name[][10]={
	L"一方通行",
	L"幻想杀手",
	L"炼金术师",
	L"变态绅士",
	L"血意少年",
	L"问题儿童",
	L"进击的巨人",
	L"艾伦·耶格尔",
	L"利威尔",
	L"薄樱鬼",
	L"滑头鬼",
	L"星矢",
	L"童虎",
	L"茅场晶彦",
	L"桐谷和人",
	L"卫宫切嗣",
	L"言峰绮礼",
	L"亚瑟王",
	L"西绪福斯",
	L"普罗米修斯",
	L"吉尔伽美什",
	L"黑崎一护",
	L"蓝染惣右介",
	L"乌尔奇奥拉·西法",
	L"路飞",
	L"索隆",
	L"银魂",
	L"MAGI",
	L"辛巴达",
	L"阿里巴巴",
	L"阿拉丁",
	L"小畑健",
	L"L",
	L"夜神月",
	L"JOJO",
	L"金刚狼",
	L"李刚",
	L"受虐狂",
	L"施虐狂",
	L"小学生",
	L"高富帅",
	L"凹凸曼",
	L"比尔盖子",
	L"绝望先生",
	L"抠脚大汉"
};

static int init = 0;

wchar_t *GetPlayerName(void)
{
	int n;
	if( init == 0 ) {
		init = 1;
		srand((unsigned int)time(NULL));
	}
	n = rand() % (sizeof(player_name)/sizeof(player_name[0]));
	return player_name[n];
}
