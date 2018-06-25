//reference: https://www.informatik.hu-berlin.de/de/forschung/gebiete/wbi/teaching/archive/ss11/vl_algorithmen/09_searching.pdf
//Author: nabackert@smcm.edu
public class FibonacciSearch extends Search {
	int len = items.length - 1;
	int fib = 2;
	int fib1 = 1;
	int fib2 = 1;
	int offset = 0;
	
	
	

	public FibonacciSearch(int[] items, int item) {
		super(items, item);
		relVars.add(fib);
		relVars.add(fib1);
		relVars.add(fib2);
	}
	@Override
	public void getNextStep() {
		
		
		//System.out.println("steps: " +steps);
		
		if(steps == 0){
			if(item == items[0]){
				index = 0;
				result=Result.EQUAL;
				return;
			}
			while(fib<len){
				fib2=fib1;
				fib1=fib;
				fib=fib1+fib2;
			}
		}
		
		
		
		
		System.out.println("fib2: " + fib2 + " Fib1: " + fib1 + " Fib: " + fib);
//		while(items[index]!=item){
			index=Math.min((offset+fib2),len);
//			if(index==len){
//				index=index-1;
//			}
			
			/*
			 * Pre: This algorithm takes in an array that is assumed to be sorted and composed of only integers,
			 * and an integer to search for.
			 * 
			 * Post: This algorithm returns the index at which the int we are looking for is found.
			 */
		if(fib <= 1){
			result = Result.NOTFOUND;
		}
		else if(item<items[index]){
			fib=fib2;
			fib1=fib1-fib2;
			fib2=fib-fib1;
			result=Result.LEFT;
		}else if(item>items[index]){
			fib=fib1;
			fib1=fib2;
			fib2=fib-fib1;
			offset=index;
			result=Result.RIGHT;
		}else if(item == items[index]){
			result=Result.EQUAL;
		}else{
			result=Result.UNDEF;
		}
		//this is so we can print the values in the GUI
		relVars.set(0, fib);
		relVars.set(1, fib1);
		relVars.set(2, fib2);
		
		}
		
//	}

}