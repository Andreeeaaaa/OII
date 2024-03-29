#include <bits/stdc++.h> 

using namespace std;

#define vi vector<int>
#define pi pair<int, int>
#define vp vector<pi>
#define ll long long int

#define MAXN 100000

vi arr;
int N, lsb_pos;

// preso da https://github.com/Andreeeaaaa/OII/blob/main/tutorial/fenwick.cpp
// (mia pagina github)


static long long MAX = LLONG_MAX;

struct Node{
	long long mi;
	long long val;
    long long lazy_set;
    long long lazy_add;

	Node() : mi(MAX), val(0), lazy_add(0), lazy_set(LLONG_MIN) {};

	void join(Node &l, Node &r) {
        mi = min(l.mi, r.mi);
		val = l.val + r.val;
	}
};

int n, real_size;	// real_size è anche l'indice del primo nodo dell'ultimo livello
vector<Node> nodes;

void is_lazy(int u, int l, int r) {
	if(nodes[u].lazy_add == 0 && nodes[u].lazy_set == LLONG_MIN) return;

	if(nodes[u].lazy_set != LLONG_MIN && nodes[u].lazy_add != 0) {
        // aumento direttamente il nodo e propago ai figli nel lazy
		nodes[u].mi = nodes[u].lazy_set;	
		// se cambio l'interno range di numeri in n --> la somma dei numeri sarà il numero di nodi incluso nel range * n
		nodes[u].val = nodes[u].lazy_set*(r-l);
		// aumento direttamente il nodo e propago ai figli nel lazy
		nodes[u].mi += nodes[u].lazy_add;
		// se aggiungo n a tutte le foglie in un range, il singolo nodo aumenta di n*range
		nodes[u].val += nodes[u].lazy_add*(r-l);
		
		if(2*u+1 < nodes.size()) {
			nodes[2*u].lazy_set = nodes[u].lazy_set;
			nodes[2*u+1].lazy_set = nodes[u].lazy_set;
			nodes[2*u].lazy_add = nodes[u].lazy_add;
			nodes[2*u+1].lazy_add = nodes[u].lazy_add;
		}

        nodes[u].lazy_add = 0;
        nodes[u].lazy_set = LLONG_MIN;
        return ;
    }

	if(nodes[u].lazy_add != 0) {
		// aumento direttamente il nodo e propago ai figli nel lazy
		nodes[u].mi += nodes[u].lazy_add;
		// se aggiungo n a tutte le foglie in un range, il singolo nodo aumenta di n*range
		nodes[u].val += nodes[u].lazy_add*(r-l);

		// non sono in una foglia, ho dei figli
		if(2*u+1 < nodes.size()) {
			// non si devono stackare lazy_updates
			// SE AGGIUNGO QUALCOSA DOPO UN SET DEVOTENERE CONTO DEL SET
			nodes[2*u].lazy_add += nodes[u].lazy_add;
			nodes[2*u+1].lazy_add += nodes[u].lazy_add;
		}

		nodes[u].lazy_add = 0;
		return;
	}

	// aumento direttamente il nodo e propago ai figli nel lazy
	nodes[u].mi = nodes[u].lazy_set;	
	// se cambio l'interno range di numeri in n --> la somma dei numeri sarà il numero di nodi incluso nel range * n
	nodes[u].val = nodes[u].lazy_set*(r-l);

	if(2*u+1 < nodes.size()) {
		// non voglio che si stackino lazy updates
		nodes[2*u].lazy_add = 0;
		nodes[2*u+1].lazy_add = 0;
		
		nodes[2*u].lazy_set = nodes[u].lazy_set;
		nodes[2*u+1].lazy_set = nodes[u].lazy_set;
	}

	nodes[u].lazy_set = LLONG_MIN;
	return;
}

void build(int u, int l, int r, vector<long long> &a) {
	// se il range è = 1, sono in una foglia
	if(r-l <= 1) {
		if(l<a.size()) {	// il numero esiste e non è solo un -INF
			nodes[u].mi = a[l];	// l è incluso, r no --> il nodo è l
			nodes[u].val = a[l];
		}
	}
	// se non sono in una foglia calcolo ricorsivamente i due figli
	else {
		build(u*2, l, (l+r)/2, a);
		build(u*2+1, (l+r)/2, r, a);
		nodes[u].join(nodes[2*u], nodes[2*u+1]);
	}
}

void init(vector<long long> a) {
	// n è la lunghezza della sequenza di numeri, realsize la potenza di due subito dopo
	n = a.size();

	real_size = 1;
	while(real_size < n) {
		real_size *= 2;
	}

	nodes.assign(2*real_size, Node());

	build(1, 0, real_size, a);
}

long long get_sum(int u, int l, int r, int x, int y) {
	// tengo conto del nodo, quindi mi serve che sia up-to-date
	is_lazy(u, l, r);
	
	// controllo se è completamente escluso
	if(l >= y || r <= x) return 0;

	// completamente incluso
	if(l>=x && r<=y) return nodes[u].val;

	// parzialmente incluso, somma dei due figli
	return (get_sum(2*u, l, (r+l)/2, x, y) + get_sum(2*u+1, (l+r)/2, r, x, y));
}

long long get_sum(int l, int r) {
	return get_sum(1, 0, real_size, l, r);
}

void add(int u, int l, int r, int x, int y, long long n) {
	// tengo conto del nodo, quindi mi serve che sia up-to-date
	is_lazy(u, l, r);
	
	// no overlap, nothing to do
	if(l >= y || r <= x) return;

	// full overlap, update node & lazy children
	// sia la somma che il minimo aumenteranno di x
	if(l >= x && r <= y) {
		// aumento direttamente il nodo e propago ai figli nel lazy
		nodes[u].mi += n;
		// se aggiungo n a tutte le foglie in un range, il singolo nodo aumenta di n*range
		nodes[u].val += n*(r-l);

		if(2*u+1 < nodes.size()) {
			nodes[2*u].lazy_add += n;
			nodes[2*u+1].lazy_add += n;
		}

		return;
	}

	// partial overlap
	add(2*u, l, (l+r)/2, x, y, n);
	add(2*u+1, (l+r)/2, r, x, y, n);
	nodes[u].join(nodes[2*u], nodes[2*u+1]);

	return;
}

void add(int l, int r, long long x) {
	add(1, 0, real_size, l, r, x);
}

void set_range(int u, int l, int r, int x, int y, ll n) {
	// tengo conto del nodo, quindi mi serve che sia up-to-date
	is_lazy(u, l, r);

	// no overlap, nothing to do
	if(l >= y || r <= x) return;

	// full overlap, update node & lazy children
	// sia la somma che il minimo aumenteranno di x
	if(l >= x && r <= y) {
		// aumento direttamente il nodo e propago ai figli nel lazy
		nodes[u].mi = n;	
		// se cambio l'interno range di numeri in n --> la somma dei numeri sarà il numero di nodi incluso nel range * n
		nodes[u].val = n*(r-l);

		// non sono una foglia --> ho dei figli
		if(2*u+1 < nodes.size()) {
			// non voglio che si stackino lazy updates
			nodes[2*u].lazy_add = 0;
			nodes[2*u+1].lazy_add = 0;
			
			nodes[2*u].lazy_set = n;
			nodes[2*u+1].lazy_set = n;
		}
		return;
	}

	// partial overlap
	set_range(2*u, l, (l+r)/2, x, y, n);
	set_range(2*u+1, (l+r)/2, r, x, y, n);
	nodes[u].join(nodes[2*u], nodes[2*u+1]);

	return;
}

void set_range(int l, int r, long long x) {
	set_range(1, 0, real_size, l, r, x);
}

long long get_max(int u, int l, int r, int x, int y) {	// y escluso
	// tengo conto del nodo, quindi mi serve che sia up-to-date
	is_lazy(u, l, r);
	
	// controllo se i limiti sono completamente esclusi
	if(l >= y || r <= x) return 0;

	// sono completamente inclusi, ritorno il valore del nodo corrente
	if(l >= x && r <= y) return nodes[u].mi;

	// se sono inclusi a metà prendo il minimo della chiamata ricorsiva ai due figli
	return max(
		get_max(2*u, l, (l+r)/2, x, y),
		get_max(2*u+1, (l+r)/2, r, x, y)
	);
}

int insert(int index, vi &fenwick) {
    lsb_pos = (int) log2(index & (~index + 1))+1;
    for(int i=0; i!=lsb_pos; i++) fenwick[index] += arr[index-i-1];
}

int res;
int prefix_sum(int a, vi &fenwick) {
    while(a>0) {
        res += fenwick[a];
        a -= a & (-a);
    }
    return res;
}

int sum_query(int a, int b, vi &fenwick) {
    if(a>b) swap(a, b);
    return (prefix_sum(b, fenwick) - prefix_sum(a, fenwick));
}

void add(int index, int a, vi &fenwick) {
    while(index<fenwick.size()) {
        // aggiungo valore
        fenwick[index] += a;
        // aggiungo potenza di due meno significativa all'index
        index += index & (-index);
    }
}

void range_add(int a, int b, int val, vi &fenwick) {
    add(a, val, fenwick);
    add(b+1, -val, fenwick);
}

int main() {
    int K;
    cin >> N >> K;
    vi fenwick(MAXN+1, 0);
    int tmp1, tmp2;
    vector<ll> overlap(N);
    set<pair<int, int>> meetings(N);

    for(int i=0; i<N; i++) {
        cin >> tmp1 >> tmp2;
        meetings.insert({tmp1, tmp2});
        range_add(tmp1, tmp2, 1, fenwick);
    }

    sort(meetings.begin(), meetings.end());

    // calcolo overlap
    for(int i=0; i<N; i++) {
        // forse l'estremo è escluso?
        overlap[i] = sum_query(meetings[i].first, meetings[i].second, fenwick);
    }

    init(overlap); // segtree

    // tolgo quello max, faccio update tree e ricomincio
    int index = 0;
    int minn = MAXN*2;
    for(int i=0; i<K; i++) {
        index = get_max(1, 0, real_size, meetings[i].first, meetings[i].second+1);
        range_add(i+1, overlap.)
    }
    
    // queries
    res = 0;

    return 0;
}