// 2022-11-17

#include "CSBP.h"
using namespace std;


void SplitString(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	v.clear();//删除原内容
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));
		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
	{
		v.push_back(s.substr(pos1));
	}
}


tuple<int, int, int> ReadData(All_Values& Values, All_Lists& Lists)
{
	ostringstream s_in, s_out;
	string in_str, out_str;
	ofstream f_out;
	string line;
	vector<string> data_inline, data_inline1, data_inline2;

	int stocks_num = -1;
	int number_of_items = -1;
	int stock_length = -1;
	int item_types_num = -1;

	// CSBB01.txt

//#pragma region CSBB01
//	
//	s_in.str("");
//	s_in << "CSBB01.txt";
//	in_str = s_in.str();
//	ifstream fin1(in_str);
//
//	if (fin1)
//	{
//		getline(fin1, line);
//		SplitString(line, data_inline, "\t"); // 第1行
//		stocks_num = atoi(data_inline[0].c_str()); // 第1行第1位：总计可用母管数量
//
//		getline(fin1, line);
//		SplitString(line, data_inline, "\t"); // 第2行
//		item_types_num = atoi(data_inline[0].c_str()); // 第2行第1位：子管种类数量
//
//		getline(fin1, line);
//		SplitString(line, data_inline, "\t"); // 第3行
//		stock_length = atoi(data_inline[0].c_str()); //  第3行第1位：母管长度
//		printf("\n\n");
//		printf("	The number of stocks = %d\n", stocks_num);
//		printf("	The number of item_types = %d\n", item_types_num);
//		printf("	The length of stock = %d\n", stock_length);
//
//		int item_index = 1;
//		for (size_t i = 0; i < item_types_num; i++) // 所有子管种类行
//		{
//			getline(fin1, line);
//			SplitString(line, data_inline, "\t");
//
//			int item_demand = atoi(data_inline[1].c_str());
//			//问题：每个母管 按照需求的长度切一刀，因而按照需求量来进行循环？
//			for (int col = 0; col < item_demand; col++) // 子管需求量
//			{
//				ItemProperties this_item;
//				this_item.length = atoi(data_inline[0].c_str()); // 子管行第1位：子管长度
//				this_item.demand = atoi(data_inline[1].c_str()); // 子管行第2位：子管需求
//				this_item.type = atoi(data_inline[2].c_str()); // 子管行第3位：子管种类
//
//				this_item.index = item_index; // 子管序号，从1开始
//				this_item.stock_index = -1; // 子管所属母板编号
//				this_item.occupied = 0;
//				Lists.all_items_list.push_back(this_item);
//				item_index++;
//			}
//			ItemTypeProperties this_item_type;
//			this_item_type.length = atoi(data_inline[0].c_str());
//			this_item_type.demand = atoi(data_inline[1].c_str());
//			this_item_type.type = atoi(data_inline[2].c_str());
//			Lists.all_item_types_list.push_back(this_item_type);
//		}
//	}
//#pragma endregion CSBB01

	// binpack.txt

#pragma region binpack

	s_in.str("");
	s_in << "C:/Users/YMZhao/Desktop/CSBP/data/binpack2.txt";
	in_str = s_in.str();
	ifstream fin2(in_str);

	if (fin2)
	{

		getline(fin2, line);
		SplitString(line, data_inline, "\t");
		stocks_num = atoi(data_inline[0].c_str());

		getline(fin2, line);
		SplitString(line, data_inline, "\t");
		number_of_items = atoi(data_inline[0].c_str());

		getline(fin2, line);
		SplitString(line, data_inline, "\t");
		stock_length = atoi(data_inline[0].c_str());

		printf("\n\n	The number of stocks = %d\n", stocks_num);
		printf("	The number of items = %d\n", number_of_items);
		printf("	The length of stock = %d\n", stock_length);

		for (size_t i = 0; i < stocks_num; i++)
		{
			StockProperties this_stock;
			this_stock.length = stock_length;
			// Lists.stock_pool_list.insert(Lists.stock_pool_list.begin(), this_stock);
		}

		int item_index = 1;
		int item_type_index = 1;

		for (size_t i = 0; i < number_of_items; i++)
		{
			getline(fin2, line);
			SplitString(line, data_inline, "\t");

			ItemProperties this_item;
			this_item.length = atoi(data_inline[0].c_str());
			this_item.demand = 1;
			this_item.type = item_type_index;
			this_item.index = item_index;
			this_item.stock_index = -1;
			this_item.occupied = 0;

			Lists.all_items_list.push_back(this_item);
			item_index++;
		}

		size_t all_items_list_size = Lists.all_items_list.size();
		vector<int> temp_item_types_list;;
		int distance_index = 0;
		vector<int>::iterator iter;

		for (int i = 0; i < all_items_list_size; i++)
		{
			int this_item_length = Lists.all_items_list[i].length;

			if (find(temp_item_types_list.begin(), temp_item_types_list.end(), this_item_length)
				== temp_item_types_list.end())
			{
				temp_item_types_list.push_back(this_item_length);

				ItemTypeProperties this_item_type;
				this_item_type.length = Lists.all_items_list[i].length;
				this_item_type.demand = 1;
				this_item_type.type = item_type_index;
				Lists.all_item_types_list.push_back(this_item_type);
				item_type_index++;
			}
			else
			{
				iter = find(temp_item_types_list.begin(), temp_item_types_list.end(), this_item_length);
				distance_index = distance(temp_item_types_list.begin(), iter);
				Lists.all_item_types_list[distance_index].demand = Lists.all_item_types_list[distance_index].demand + 1;
			}
		}

		size_t item_types_num = Lists.all_item_types_list.size();
		printf("	The number of item_type is %zd\n", item_types_num);
	}

#pragma endregion binpack

	ItemProperties  VP;
	size_t all_items_list_size = Lists.all_items_list.size();

	for (size_t i = 0; i < all_items_list_size - 1; i++)
	{
		for (size_t j = i + 1; j < all_items_list_size; j++)
		{
			if (Lists.all_items_list[i].length < Lists.all_items_list[j].length)
			{
				VP = Lists.all_items_list[i];
				Lists.all_items_list[i] = Lists.all_items_list[j];
				Lists.all_items_list[j] = VP;
			}
		}
	}

	tuple<int, int, int> flag(stocks_num, number_of_items, stock_length);
	return flag;
}

