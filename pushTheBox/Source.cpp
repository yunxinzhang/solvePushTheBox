#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <set>
#include <queue>
#include <ctime>
using namespace std;
class Pos {
public:
	int x;
	int y;
	Pos():x(-1),y(-1){}
	Pos(int x, int y):x(x),y(y){}
	bool operator<(const Pos &p)const {
		if (x < p.x || (x == p.x&&y < p.y))
			return true;
		else
			return false;
	}
	bool operator==(const Pos &p)const {
		if (x == p.x  && y == p.y)
			return true;
		else
			return false;
	}
};
//    d
//	b	a
//	  c
Pos dir[4] = { Pos(0,1), Pos(0,-1) ,Pos(1,0),Pos(-1,0) };   

class Map {
public:
	int row;
	int col;
	int iter;
	vector<vector<char>> maze;
	vector<vector<bool>> visited;
	Map * pre;
	string mv;
	Pos man;
	set<Pos> boxes;
	set<Pos> flags;
	static map<set<Pos>,set<Pos>> state; // box --> man
	bool manHasPath(Pos p) {
		initVisited();
		return go(man, p);
	};
	bool win() { return boxes == flags; }
	void initVisited() {
		visited.clear();
		for (int i = 0; i < row; ++i) {
			vector<bool> v;
			for (int j = 0; j < col; ++j) {
				v.push_back(false);
			}
			visited.push_back(v);
		}
	}
	// go使用前需要先初始化visited
	bool go(Pos m, Pos p) {
		/*cout << "go >> " << m.x << "\t" << m.y << endl;
		for (auto it : boxes) {
			cout << it.x << "," << it.y << "  ";
		}
		cout << endl;*/
		visited[m.x][m.y] = true;
		if (m.x == p.x&&m.y == p.y)return true;
		for (int i = 0; i < 4; ++i) {
			int newx = m.x + dir[i].x;
			int newy = m.y + dir[i].y;
			if (newx >= 0 && newx < maze.size() && newy>=0 
						&& newy < maze[0].size() && maze[newx][newy] != 'x'
						&&(boxes.find(Pos(newx,newy))==boxes.end())
						&&!visited[newx][newy]) {
				Pos m2(newx, newy);
				
				if (go(m2, p))return true;
			}
		}
		return false;
	}
	bool isLinked(Pos m, Pos p, set<Pos> newbox) {
		initVisited();
		return go(m, p, newbox);
	}
	bool go(Pos m, Pos p, set<Pos> newbox) {
		/*cout << "go >> " << m.x << "\t" << m.y << endl;
		for (auto it : newbox) {
			cout << it.x << "," << it.y << "  ";
		}
		cout << endl;*/
		visited[m.x][m.y] = true;
		if (m.x == p.x&&m.y == p.y)return true;
		for (int i = 0; i < 4; ++i) {
			int newx = m.x + dir[i].x;
			int newy = m.y + dir[i].y;
			if (newx >= 0 && newx < maze.size() && newy >= 0
				&& newy < maze[0].size() && maze[newx][newy] != 'x'
				&& (newbox.find(Pos(newx, newy)) == newbox.end())
				&& !visited[newx][newy]) {
				Pos m2(newx, newy);

				if (go(m2, p, newbox))return true;
			}
		}
		return false;
	}
	Map(string path) {
		pre = NULL;
		mv = "";
		iter = 0;
		ifstream fin("m.txt");
		if (!fin) {
			cout << "read map file error!" << endl;
			getchar();
		}
		fin >> row >> col;
		char c;
		for (int i = 0; i < row; ++i) {
			vector<bool> v;
			for (int j = 0; j < col; ++j) {
				v.push_back(false);
			}
			visited.push_back(v);
		}
		for (int i = 0; i < row; ++i) {
			vector<char> v;
			for (int j = 0; j < col; ++j) {
				fin >> c;
				v.push_back(c);
				if (c == '$') { // man
					man.x = i; 
					man.y = j; 
				}
				else if (c == 'f') { // flag
					flags.insert(Pos(i, j));
				}
				else if (c == 'b') { // box
					boxes.insert(Pos(i, j));
				}
				else if (c == 'o') { // flag & box
					flags.insert(Pos(i, j));
					//maze[i][j] = 'b';
					boxes.insert(Pos(i, j));
				}
				else if (c == 'y') { // flag & man
					flags.insert(Pos(i, j));
					//maze[i][j] = 'f';
					man.x = i; 
					man.y = j;
				}

			}
			maze.push_back(v);
		}
		
		for (auto it : flags) {
			maze[it.x][it.y] = '=';
		}
		for (auto it : boxes) {
			maze[it.x][it.y] = 'b';
		}
		maze[man.x][man.y] = '=';
		fin.close();
	}
	bool inState(set<Pos> s, Pos t) {
		if (state.find(s) == state.end())return false;
		else {
			//Map m(*this); m.boxes = s;
			
			//m.disp();
			for (auto it : state.at(s)) {
				//initVisited();
				
				//cout << endl;
				if (isLinked(t, it, s)) {
					//cout << "t >>" << t.x << "," << t.y << "," << it.x << "," << it.y << endl;
					return true;
				}
			}
			return false;
		}
	}
	
	//限制的条件不够充分
	
	
	bool cantSolve() {
		//cout << "cant_method" << endl;
		int minx=row-1, maxx=0, miny=col-1, maxy = 0;
		for (auto it : boxes) {
			if (it.x < minx) {
				minx = it.x;
			}
			if (it.x > maxx) {
				maxx = it.x;
			}
			if (it.y < miny) {
				miny = it.y;
			}
			if (it.y > maxy) {
				maxy = it.y;
			}
		}
		int fminx = row - 1, fmaxx = 0, fminy = col - 1, fmaxy = 0;
		for (auto it : flags) {
			if (it.x < fminx) {
				fminx = it.x;
			}
			if (it.x > fmaxx) {
				fmaxx = it.x;
			}
			if (it.y < fminy) {
				fminy = it.y;
			}
			if (it.y > fmaxy) {
				fmaxy = it.y;
			}
		}
		if ((fminx > 0 && minx == 0)||(fmaxx<row-1 && maxx==row-1)||(fminy>0&&miny==0)||(fmaxy<col-1&&maxy==col-1)) return true;
		for (auto it : boxes) {
			int x1 = it.x + dir[0].x;
			int y1 = it.y + dir[0].y;

			int x2 = it.x + dir[1].x;
			int y2 = it.y + dir[1].y;

			int x3 = it.x + dir[2].x;
			int y3 = it.y + dir[2].y;

			int x4 = it.x + dir[3].x;
			int y4 = it.y + dir[3].y;
			//    d
			//	b	a
			//	  c
			/*cout << "x,y" << it.x << "," << it.y << endl;
			cout << x1 << "," << y1 << endl;
			cout << x2 << "," << y2 << endl;
			cout << x3 << "," << y3 << endl;
			cout << x4 << "," << y4 << endl;
			cout << (flags.find(it) == flags.end()) << endl;
			cout << (y1 >= col || maze[x1][y1] == 'x') << endl;
			cout << (x3 >= row || maze[x3][y3] == 'x') << endl;*/
			if ((y1 >=col  || maze[x1][y1] == 'x') && (x3 >= row || maze[x3][y3] == 'x') && flags.find(it) == flags.end())return true;
			if ((y1 >= col || maze[x1][y1] == 'x') && (x4 < 0 || maze[x4][y4] == 'x') && flags.find(it) == flags.end())return true;
			if ((y2 < 0 || maze[x2][y2] == 'x') && (x3 >= row || maze[x3][y3] == 'x') && flags.find(it) == flags.end())return true;
			if ((y2 < 0 || maze[x2][y2] == 'x') && (x4 <0 || maze[x4][y4] == 'x') && flags.find(it) == flags.end())return true;

		}
		return false;
	}
	void disp() {
		cout << "----------------" << endl;
		for (auto it : boxes) {
			cout << it.x << "\t" << it.y << endl;;
		}
	}
	
	public :void optimalPath() {
		Map * linkOp = this;
		Map * inChain = this;
		Map * mv = pre;
		while (inChain->pre != NULL) {
			cout << 1 << endl;
			linkOp = inChain->pre;
			mv = inChain->pre;
			while (mv != NULL) {
				//cout << 2 << endl;
				mv = mv->pre;
				/*cout << "==================" << endl;
				for (auto it : mv->boxes) {
					cout << it.x << "," << it.y <<" ";
				}
				cout << endl;
				cout << "------------------" << endl;
				for (auto it : linkOp->boxes) {
					cout << it.x << "," << it.y << " ";
				}
				cout << endl;
				cout << "\n" << endl;*/
				
				if (mv != NULL) {
					cout << mv->state.size() << endl;
					cout <<">>>>>>>>>>>>>> "<< ((mv->boxes) == (linkOp->boxes)) << endl;
				}
				if (mv!=NULL&&(mv->boxes) == (linkOp->boxes) && linkOp->manHasPath(mv->man)) {
					cout << 3 << endl;
					linkOp = mv;
				}
			}
			inChain->pre = linkOp;	
			inChain = inChain->pre;
		}		
	}
};
map<set<Pos>, set<Pos>> Map::state;
// 广度优先
bool solve2(Map &m) {
	++m.iter;
	queue<Map> list;
	//cout <<"===================iter >> " <<m.iter << endl;
	//m.disp();
	if (m.state.find(m.boxes) == m.state.end()) {
		set<Pos> s;
		string str = "";
		s.insert(m.man);
		m.state.insert(pair<set<Pos>, set<Pos>>(m.boxes, s));

	}
	else {
		bool finded = false;
		for (auto it : m.state.at(m.boxes)) {
			if (m.manHasPath(Pos(it.x, it.y))) {
				finded = true;
				break;
			}
		}
		if (!finded)
			m.state.find(m.boxes)->second.insert(m.man);
	}
	if (m.win()) {
		cout << "solved" << endl;
		vector<string> res;
		//m.optimalPath();
		Map *mv = &m;
		res.push_back(mv->mv);
		//cout << mv->mv << endl;
		while (mv->pre != NULL) {
			mv = mv->pre;
			res.push_back(mv->mv);
			//cout << mv->mv << endl;

		}
		cout << "结果 >> " << endl;
		for (int i = 0; i < m.row; ++i) {
			for (int j = 0; j < m.col; ++j) {
				cout << m.maze[i][j] << '\t';
			}
			cout << "\t\t" << i;
			cout << endl;
		}
		for (int i = res.size() - 1; i >= 0; --i) {
			cout << res[i] << endl;
		}
		return true;
	}
	if (m.cantSolve()) {
		return false;
	}
	list.push(m);
	//cout << m.flags.begin()->x << ":" << m.flags.begin()->y << endl;
	cout << "搜索中..." << endl;
	int step = 0;
	long tm1,tm2=clock(), tmlast;
	while (list.size() > 0) {
		//cout <<"List >> "<< list.size() << endl;
		
		Map *mm = new Map(list.front());
		if (step != mm->iter) {
			tm1 = clock();
			cout << (tm1 - tm2) << endl;
			tm2 = tm1;
			
			cout << "第" << step << "步" << endl;
		}
		step = mm->iter;
		//cout <<"fifo"<< mm->iter<<" >> "<< mm->mv << endl;
		/*for (auto itin : mm->boxes) {
			cout << itin.x << "," << itin.y << ";";
		}
		cout << endl;*/
		list.pop();
		if (mm->state.find(mm->boxes) == mm->state.end()) {
			set<Pos> s;
			string str = "";
			s.insert(mm->man);
			mm->state.insert(pair<set<Pos>, set<Pos>>(mm->boxes, s));

		}
		else {
			bool finded = false;
			for (auto it : mm->state.at(mm->boxes)) {
				if (mm->manHasPath(Pos(it.x, it.y))) {
					finded = true;
					break;
				}
			}
			if (!finded)
				mm->state.find(mm->boxes)->second.insert(mm->man);
		}
		if (mm->win()) {

			cout << "solved" << endl;
			vector<string> res;
			//m.optimalPath();
			Map *mv = mm;
			res.push_back(mv->mv);
			//cout << mv->mv << endl;
			while (mv->pre != NULL) {
				mv = mv->pre;
				res.push_back(mv->mv);
				//cout << mv->mv << endl;

			}
			
			for (int i = 0; i < m.row; ++i) {
				for (int j = 0; j < m.col; ++j) {
					cout << m.maze[i][j] << '\t';
				}
				cout << "\t<----  " << i;
				cout << endl;
			}
			cout << "结果如下： " << endl;
			for (int i = res.size() - 1; i >= 0; --i) {
				cout << res[i] << endl;
			}
			return true;
		}
		if (mm->cantSolve()) {
			//cout << "not this" << endl;
			delete mm;
			continue;
		}
		int newstepnum = 0;
		for (auto it : mm->boxes) {
			for (int j = 0; j < 4; ++j) {
				int newx = it.x + dir[j].x;
				int newy = it.y + dir[j].y;
				int manx = it.x - dir[j].x;
				int many = it.y - dir[j].y;
				//cout << newx << "---"<<newy << endl;
				if (newx >= 0 && newx < mm->maze.size() && newy >= 0
					&& newy < mm->maze[0].size() && mm->maze[newx][newy] != 'x'
					&&mm->maze[newx][newy] != 'b' && mm->maze[newx][newy] != 'o'
					&& manx >= 0 && manx < mm->maze.size() && many >= 0
					&& many < mm->maze[0].size() && mm->maze[manx][many] != 'x'
					&&mm->maze[manx][many] != 'b' && mm->maze[manx][many] && 'o' && mm->manHasPath(Pos(manx,many))) {
					set<Pos> bpos = mm->boxes;
					bpos.erase(Pos(it.x, it.y));

					bpos.insert(Pos(newx, newy));
					Pos mpos(it.x, it.y);
					
		
					if (!mm->inState(bpos, mpos)) {
						Map *m2 = new Map(*mm);
						++newstepnum;
						m2->iter++;
						m2->mv = to_string(it.x) + "," + to_string(it.y) + "-->" + to_string(newx) + "," + to_string(newy);
						m2->pre = mm;
						m2->maze[newx][newy] = 'b';
						m2->maze[it.x][it.y] = '=';
						m2->boxes = bpos;
						m2->man = mpos;
						//if(m2->inState(bpos,mpos))
						list.push(*m2);
						//else 
						//	delete m2;
					}
				}
			}
	
		}
		if (newstepnum == 0)delete mm;
	}
	cout << "找不到答案" << endl;
	return false;
}
// 深度有限不是最少步数
bool solve(Map &m) {
	++m.iter;
	//cout <<"===================iter >> " <<m.iter << endl;
	//m.disp();
	if (m.state.find(m.boxes) == m.state.end()) {
		set<Pos> s;
		string str = "";
		s.insert(m.man);
		m.state.insert(pair<set<Pos>, set<Pos>>(m.boxes, s));

	}
	else {
		bool finded = false;
		for (auto it : m.state.at(m.boxes)) {
			if (m.manHasPath(Pos(it.x, it.y))) {
				finded = true;
				break;
			}
		}
		if (!finded)
			m.state.find(m.boxes)->second.insert(m.man);
	}
	if (m.win()) {
		cout << "solved" << endl;
		m.optimalPath();
		Map *mv = &m;
		while (mv->pre != NULL) {
			mv = mv->pre;
			cout << mv->mv << endl;
			
		}
		return true;
	}
	if (m.cantSolve()) { 
		return false;
	}
	
	
	for (auto it: m.boxes) {
		for (int j = 0; j < 4; ++j) {
			int newx = it.x + dir[j].x;
			int newy = it.y + dir[j].y;
			int manx = it.x - dir[j].x;
			int many = it.y - dir[j].y;
			if (newx >= 0 && newx < m.maze.size() && newy >= 0
				&& newy < m.maze[0].size() && m.maze[newx][newy] != 'x'
				&&m.maze[newx][newy] != 'b' && m.maze[newx][newy] && 'o'
				&& manx >= 0 && manx < m.maze.size() && many >= 0
				&& many < m.maze[0].size() && m.maze[manx][many] != 'x'
				&&m.maze[manx][many] != 'b' && m.maze[manx][many] && 'o') {
				set<Pos> bpos = m.boxes;
				bpos.erase(Pos(it.x, it.y));
				
				bpos.insert(Pos(newx, newy));
				Pos mpos(it.x, it.y);
				/*for (auto init : m.state) {
					for (auto initin : init.first) {
						cout << "(" << initin.x << "," << initin.y<< ")	";
					}
					for (auto initin : init.second) {
						cout << "{" << initin.x << "," << initin.y << "} ";
					}
					cout << endl;
				}
				for (auto initin : bpos) {
					cout << "==>[" << initin.x << "," << initin.y << "]	";
				}*/
				//cout << endl;
				//cout << "instate<<" << m.inState(bpos, mpos) << endl;;
				//cout << "instate<<" << !m.inState(bpos, mpos) << endl;;
				//cout << "instate<<" << m.inState(bpos, mpos) << endl;;
				//cout << "instate<<" << !m.inState(bpos, mpos) << endl;;
				//cout << "state size >>" << m.state.size() << endl;
				if (!m.inState(bpos, mpos)) {
					Map *m2 = new Map(m);
					m.mv = to_string(it.x) + "," + to_string(it.y) + "-->" + to_string(newx) + "," + to_string(newy);
					m2->pre = &m;
					m2->maze[newx][newy] = 'b';
					m2->maze[it.x][it.y] = '=';
					m2->boxes = bpos;
					m2->man = mpos;
					if (solve(*m2)) {
						return true;
					};
				}
			}
		}
	}
	return false;
}
int main() {
	Map m("m.txt");
	set<Pos> ss;
	//ss.insert(Pos(1, 3));
	//ss.insert(Pos(2, 5));
	//cout << m.isLinked(Pos(1, 2), Pos(1, 1), ss) << endl;;
	//cout << m.isLinked(Pos(1, 2), Pos(1, 4), ss) << endl;;
	//cout << m.go(Pos(1, 2), Pos(1, 4)) <<endl;
	//Pos p(0, 0);
	//cout << m.man.x << "," << m.man.y << endl;
	solve2(m);
	//释放内存的操作缺少
	/*cout << m.manHasPath(p) << endl;
	map<set<Pos>, int>a;
	set<Pos>se;
	se.insert(p);
	a.insert(pair<set<Pos>, int>(se, 199));
	Pos q(0, 0);
	set<Pos>se2;
	se2.insert(q);
	cout << a.at(se2) << endl;*/
	getchar();
}