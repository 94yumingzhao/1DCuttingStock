// 2023-03-16

#include "CSCG.h"
using namespace std;


void SplitString(const string& line_string, vector<string>& string_list, const string& data_string) {
	string::size_type pos1, pos2;
	pos2 = line_string.find(data_string);
	pos1 = 0;
	string_list.clear();

	while (string::npos != pos2) {
		string_list.push_back(line_string.substr(pos1, pos2 - pos1));
		pos1 = pos2 + data_string.size();
		pos2 = line_string.find(data_string, pos1);
	}

	if (pos1 != line_string.length()) {
		string_list.push_back(line_string.substr(pos1));
	}
}

tuple<int, int, int> ReadData(All_Values& Values, All_Lists& Lists) {
	ostringstream s_in, s_out;
	string in_str, out_str;
	ofstream f_out;
	string line;
	vector<string> data_inline, data_inline1, data_inline2;

	int stocks_num = -1;
	int items_num = -1;
	int stock_length = -1;
	int item_types_num = -1;

	/*
	s_in.str("");
	s_in << "C:/Users/YMZhao/Desktop/1DCSP/1DCG/CSBB01.txt";
	in_str = s_in.str();
	ifstream fin1(in_str);

	if (fin1) {
		getline(fin1, line);
		SplitString(line, data_inline, "\t");
		stocks_num = atoi(data_inline[0].c_str());

		getline(fin1, line);
		SplitString(line, data_inline, "\t");
		item_types_num = atoi(data_inline[0].c_str());

		getline(fin1, line);
		SplitString(line, data_inline, "\t");
		stock_length = atoi(data_inline[0].c_str());
		printf("\n\n");
		printf("	The number of stocks = %d\n", stocks_num);
		printf("	The number of item_types = %d\n", item_types_num);
		printf("	The length of a stock = %d\n", stock_length);

		int item_index = 1;
		for (int k = 0; k < item_types_num; k++) {
			getline(fin1, line);
			SplitString(line, data_inline, "\t");

			int item_demand = atoi(data_inline[1].c_str());

			for (int m = 0; m < item_demand; m++) {
				Item_Stc this_item;
				this_item.length = atoi(data_inline[0].c_str());
				this_item.idx = item_index;

				Lists.all_items_list.push_back(this_item);
				item_index++;
			}
			Item_Type_Stc this_item_type;
			this_item_type.item_type_length = atoi(data_inline[0].c_str());
			this_item_type.item_type_demand = atoi(data_inline[1].c_str());
			this_item_type.item_type_idx = atoi(data_inline[2].c_str());
			Lists.all_item_types_list.push_back(this_item_type);
		}
	}
	*/

	// binpack.txt

	s_in.str("");
	s_in << "C:/Users/YMZhao/Desktop/1DCSP/1DCG/binpack2.txt";
	in_str = s_in.str();
	ifstream fin2(in_str);

	if (fin2)
	{
		getline(fin2, line);
		SplitString(line, data_inline, "\t");
		stocks_num = atoi(data_inline[0].c_str()); // line 1 col 1

		getline(fin2, line);
		SplitString(line, data_inline, "\t");
		items_num = atoi(data_inline[0].c_str()); // line 2 col 1

		getline(fin2, line);
		SplitString(line, data_inline, "\t");
		stock_length = atoi(data_inline[0].c_str()); // line 3 col 1

		printf("\n\n	The number of stocks = %d\n", stocks_num);
		printf("	The number of items = %d\n", items_num);
		printf("	The length of a stock = %d\n", stock_length);

		int item_index = 1;
		int item_type_index = 1;

		// store all items' info
		for (int k = 0; k < items_num; k++)
		{
			getline(fin2, line); // start from line 4
			SplitString(line, data_inline, "\t");

			Item_Stc this_item;
			this_item.idx = item_index;
			this_item.length = atoi(data_inline[0].c_str());

			Lists.all_items_list.push_back(this_item);
			item_index++;
		}

		int items_num = Lists.all_items_list.size();
		vector<int> temp_item_types_list;;
		int distance_index = 0;
		vector<int>::iterator iter;

		// Find all item_types from all items.
		for (int k = 0; k < items_num; k++)
		{
			// Items that have the same length belong to the same item_type
			int this_item_length = Lists.all_items_list[k].length;

			if (find(temp_item_types_list.begin(), temp_item_types_list.end(), this_item_length)
				== temp_item_types_list.end()) // if a new item_type is find
			{
				temp_item_types_list.push_back(this_item_length);

				Item_Type_Stc this_item_type; // Init this new item_type
				this_item_type.item_type_length = Lists.all_items_list[k].length;
				this_item_type.item_type_demand = 1;
				this_item_type.item_type_idx = item_type_index;

				Lists.all_item_types_list.push_back(this_item_type); // store this new item_type

				item_type_index++;
			}
			else // if this item_type has already been find
			{
				iter = find(temp_item_types_list.begin(), temp_item_types_list.end(), this_item_length);
				distance_index = distance(temp_item_types_list.begin(), iter);

				Lists.all_item_types_list[distance_index].item_type_demand =
					Lists.all_item_types_list[distance_index].item_type_demand + 1; // this item_type's demand+1
			}
		}

		int item_types_num = Lists.all_item_types_list.size();
		printf("	The number of item_type is %d\n", item_types_num);
	}
	

	// Sort all items according to their length
	Item_Stc  temp_item;
	for (int k = 0; k < items_num - 1; k++) {
		for (int m = k + 1; m < items_num; m++) {
			if (Lists.all_items_list[k].length < Lists.all_items_list[m].length) {
				temp_item = Lists.all_items_list[k];
				Lists.all_items_list[k] = Lists.all_items_list[m];
				Lists.all_items_list[m] = temp_item;
			}
		}
	}

	tuple<int, int, int> flag(stocks_num, items_num, stock_length);
	return flag;
}

