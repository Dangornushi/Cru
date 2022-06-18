int sub (int x) {
	if (x>0) {
		return sub(x-1); 
	}
	return x; 
}int main () {
	int s = sub(12);
	return s; 
}
int start(void) {
	return main();
}