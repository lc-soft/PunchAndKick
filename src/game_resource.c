#include <LCUI_Build.h>
#include LC_LCUI_H
#include LC_GRAPH_H

#include "game.h"

#define HEADER_MARK_TEXT	"LC-GAMES Graphics Resource File"
#define BLOCK_MARK_TEXT		"LC-GAMES Graphics Data Block"
#define MARK_SIZE		sizeof(MARK_TEXT)
#define MAX_MARK_TEXT_LEN	64
#define MAX_TEXT_LEN		256

typedef struct ResFileHeaderData_ {
	char mark[MAX_MARK_TEXT_LEN];
	int total_number;
	char class_name[MAX_TEXT_LEN];
} ResFileHeaderData;

typedef struct GraphResDataBlock_ {
	char mark[MAX_MARK_TEXT_LEN];
	char name[MAX_TEXT_LEN];
	int w, h;
	int color_type;
} GraphResDataBlock;

typedef struct GraphResItem_ {
	int id;
	char name[MAX_TEXT_LEN];
	LCUI_Graph graph;
} GraphResItem;

typedef struct GraphResClass_ {
	int id;
	char class_name[MAX_TEXT_LEN];
	LCUI_Queue graph_res;
} GraphResClass;

static LCUI_BOOL library_init = FALSE;
static LCUI_Queue resource_library;

static void GraphResClass_Destroy( void *arg )
{
	GraphResClass *res_class;
	res_class = (GraphResClass*)arg;
	Queue_Destroy( &res_class->graph_res );
}

/** 初始化 */
LCUI_API int GameGraphRes_Init( void )
{
	if( library_init ) {
		return -1;
	}
	Queue_Init( &resource_library, sizeof(GraphResClass), GraphResClass_Destroy );
	library_init = TRUE;
	return 0;
}

/** 释放全部资源 */
LCUI_API void GameGraphRes_FreeAll( void )
{
	library_init = FALSE;
	Queue_Destroy( &resource_library );
}

static GraphResItem *GameGraphRes_FindItem( GraphResClass *res_class, int item_id )
{
	int i, n;
	GraphResItem *p_item_data;

	Queue_Lock( &res_class->graph_res );
	n = Queue_GetTotal( &res_class->graph_res );
	for(i=0; i<n; ++i) {
		p_item_data = (GraphResItem*)Queue_Get( &res_class->graph_res, i );
		if( !p_item_data ) {
			continue;
		}
		if( p_item_data->id == item_id ) {
			Queue_Unlock( &res_class->graph_res );
			return p_item_data;
		}
	}
	Queue_Unlock( &res_class->graph_res );
	return NULL;
}

static GraphResClass *GameGraphRes_FindClass( int class_id )
{
	int i, n;
	GraphResClass *p_class_data;

	Queue_Lock( &resource_library );
	n = Queue_GetTotal( &resource_library );
	for(i=0; i<n; ++i) {
		p_class_data = (GraphResClass*)Queue_Get( &resource_library, i );
		if( !p_class_data ) {
			continue;
		}
		if( p_class_data->id == class_id ) {
			Queue_Unlock( &resource_library );
			return p_class_data;
		}
	}
	Queue_Unlock( &resource_library );
	return NULL;
}

/** 释放指定类名的资源 */
LCUI_API int GameGraphRes_Free( const char *class_name )
{
	int class_id;
	GraphResClass *res_class;
	class_id = BKDRHash( class_name );
	res_class = GameGraphRes_FindClass( class_id );
	if( res_class ) {
		Queue_Destroy( &res_class->graph_res );
		return 0;
	}
	return -1;
}

static void GraphResItem_Destroy( void *arg )
{
	GraphResItem *item;
	item = (GraphResItem*)arg;
	Graph_Free( &item->graph );
}

/** 添加一个新资源类，返回该资源类的标识号 */
LCUI_API int GameGraphRes_AddClass( const char *class_name )
{
	int class_id;
	GraphResClass class_data, *p_class_data;

	class_id = BKDRHash( class_name );
	p_class_data = GameGraphRes_FindClass( class_id );
	if( p_class_data == NULL ) {
		class_data.id = class_id;
		strncpy( class_data.class_name, class_name, MAX_TEXT_LEN );
		Queue_Init(	&class_data.graph_res,
				sizeof(GraphResItem),
				GraphResItem_Destroy 
		);
		Queue_Lock( &resource_library );
		Queue_Add( &resource_library, &class_data );
		Queue_Unlock( &resource_library );
	}
	return class_id;
}

LCUI_API int GameGraphRes_AddGraph( int class_id, const char *name, LCUI_Graph *graph )
{
	int item_id;
	GraphResItem *p_item, item_data;
	GraphResClass *p_class_data;

	p_class_data = GameGraphRes_FindClass( class_id );
	DEBUG_MSG("find class resources, id: %d, addr: %p\n", class_id, p_class_data);
	if( p_class_data == NULL ) {
		return -1;
	}
	item_id = BKDRHash( name );
	DEBUG_MSG("graph item, name: %s, id: %d\n", name, item_id);
	p_item = GameGraphRes_FindItem( p_class_data, item_id );
	if( p_item == NULL ) {
		DEBUG_MSG("add new graph item\n");
		item_data.id = item_id;
		strncpy( item_data.name, name, MAX_TEXT_LEN );
		item_data.graph = *graph;
		Queue_Lock( &p_class_data->graph_res );
		Queue_Add( &p_class_data->graph_res, &item_data );
		Queue_Unlock( &p_class_data->graph_res );
	}
	return 0;
}

/** 从文件内载入资源 */
LCUI_API int GameGraphRes_LoadFromFile( const char *filepath )
{
	FILE *fp;
	int count, i, j, k, class_id, n_pixel;
	ResFileHeaderData header_data;
	GraphResDataBlock data_block;
	LCUI_Graph graph_buff;

	fp = fopen( filepath, "rb" );
	if( fp == NULL ) {
		return -1;
	}
	count = fread( &header_data, sizeof(ResFileHeaderData), 1, fp );
	if( count < 1 ) {
		fclose( fp );
		return -2;
	}
	/* 验证文件头中的标记文本 */
	if( strcmp( header_data.mark, HEADER_MARK_TEXT ) != 0 ) {
		return -3;
	}
	if( header_data.total_number <= 0 ) {
		return -4;
	}
	DEBUG_MSG("class name: %s\n", header_data.class_name);
	class_id = GameGraphRes_AddClass( header_data.class_name );
	for(i=0; i<header_data.total_number; ++i) {
		count = fread( &data_block, sizeof(GraphResDataBlock), 1, fp );
		if( count < 1 ) {
			break;
		}
		/* 验证数据块的标记文本 */
		if( strcmp( data_block.mark, BLOCK_MARK_TEXT ) != 0 ) {
			/* 将文件指针移向读该数据块前的位置的下个位置 */
			fseek( fp, SEEK_CUR, -(int)(sizeof(GraphResDataBlock)-1) );
			--i;
			continue;
		}
		Graph_Init( &graph_buff );
		graph_buff.color_type = data_block.color_type;
		if( graph_buff.color_type == COLOR_TYPE_RGBA ) {
			k = 4;
		} else {
			k = 3;
		}
		Graph_Create( &graph_buff, data_block.w, data_block.h );
		DEBUG_MSG("read graph, color type: %d, width: %d, height: %d\n",
			graph_buff.color_type, data_block.w, data_block.h );
		n_pixel = data_block.w * data_block.h;
		/* 读取像素数据 */
		for(j=0; j<k; ++j) {
			count = fread(	graph_buff.rgba[j], 
					sizeof(unsigned char), n_pixel, fp );
			if( count < n_pixel ) {
				goto error_exit;
			}
		}
		GameGraphRes_AddGraph( class_id, data_block.name, &graph_buff );
	}
	fclose( fp );
	return 0;

error_exit:
	fclose( fp );
	return -5;
}

/** 将指定类资源写入至文件 */
LCUI_API int GameGraphRes_WriteToFile( const char *filepath, const char *class_name )
{
	FILE *fp;
	GraphResItem *p_item;
	GraphResDataBlock data_block;
	GraphResClass *p_class_data;
	ResFileHeaderData header_data;
	int i, n, class_id, n_pixel, count, j, k;

	class_id = BKDRHash( class_name );
	p_class_data = GameGraphRes_FindClass( class_id );
	DEBUG_MSG("find class resources...\n");
	if( p_class_data == NULL ) {
		DEBUG_MSG("class resources not found!\n");
		return -1;
	}
	DEBUG_MSG("class resources: %p\n", p_class_data);
	fp = fopen( filepath, "wb+" );
	if( fp == NULL ) {
		return -2;
	}

	strncpy( header_data.mark, HEADER_MARK_TEXT, MAX_MARK_TEXT_LEN );
	strncpy( header_data.class_name, class_name, MAX_TEXT_LEN );
	header_data.total_number = Queue_GetTotal( &p_class_data->graph_res );
	/* 先将文件头数据写进去 */
	count = fwrite( &header_data, sizeof(ResFileHeaderData), 1, fp );
	DEBUG_MSG("write file header data...\n");
	if( count < 1 ) {
		DEBUG_MSG("file header data are write error!\n");
		fclose( fp );
		return -3;
	}
	Queue_Lock( &p_class_data->graph_res );
	n = Queue_GetTotal( &p_class_data->graph_res );
	strncpy( data_block.mark, BLOCK_MARK_TEXT, MAX_MARK_TEXT_LEN );
	DEBUG_MSG("total number of graph item: %d\n", n);
	for(i=0; i<n; ++i) {
		p_item = (GraphResItem*)Queue_Get( &p_class_data->graph_res, i );
		if( !p_item ) {
			continue;
		}
		/* 准备本张图像的信息 */
		data_block.color_type = p_item->graph.color_type;
		data_block.w = p_item->graph.w;
		data_block.h = p_item->graph.h;
		strncpy( data_block.name, p_item->name, MAX_TEXT_LEN );
		DEBUG_MSG("write graph item: %p, name: %s, size: %d,%d\n",
			p_item, p_item->name, data_block.w, data_block.h );
		/* 写入本张图像的信息 */
		count = fwrite( &data_block, sizeof(GraphResDataBlock), 1, fp );
		if( count < 1 ) {
			goto error_exit;
		}
		n_pixel = data_block.w * data_block.h;
		if( p_item->graph.color_type == COLOR_TYPE_RGBA ) {
			k = 4;
		} else {
			k = 3;
		}
		DEBUG_MSG("write graph pixel data, total pixel: %d\n", n_pixel );
		/* 写入像素数据 */
		for(j=0; j<k; ++j) {
			count = fwrite(	p_item->graph.rgba[j],
						sizeof(unsigned char),
						n_pixel, fp );
			if( count < n_pixel ) {
				goto error_exit;
			}
		}
	}
	Queue_Unlock( &p_class_data->graph_res );
	fclose( fp );
	DEBUG_MSG("every thing is OK!\n" );
	return 0;

error_exit:;
	Queue_Unlock( &p_class_data->graph_res );
	fclose( fp );
	return -4;;
}

/** 在指定资源类里面获取指定名称的资源 */
LCUI_API int GameGraphRes_GetGraph(	const char *class_name,
					const char *name,
					LCUI_Graph *buff )
{
	GraphResItem *p_item;
	GraphResClass *p_class_data;
	int class_id, item_id;

	class_id = BKDRHash( class_name );
	DEBUG_MSG("find class resources...\n");
	p_class_data = GameGraphRes_FindClass( class_id );
	if( p_class_data == NULL ) {
		DEBUG_MSG("class resources not found!\n");
		return -1;
	}
	item_id = BKDRHash( name );
	p_item = GameGraphRes_FindItem( p_class_data, item_id );
	DEBUG_MSG("graph item: %p\n",p_item );
	if( p_item == NULL ) {
		return -2;
	}
	*buff = p_item->graph;
	return 0;
}
