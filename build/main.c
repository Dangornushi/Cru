int fib (int n) {
	if (n<2) {
		return n;
	}
	return fib(n-2)+fib(n-1);
}int main () {
	return fib(10);
}
int start(void) {
	return main();
}