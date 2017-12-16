import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Scanner;

/**
 * @file inside.java
 * @author Jeremy Timperio jmtimper
 * Uses multiple threads to run inside program
 */
public class Inside {

	//counts the number of points inside the circle
	private static volatile int count;

	//list of all Circle objects
	private static volatile ArrayList<Circle> cList;

	//number of threads that should be used in program
	private static volatile int threads;

	/**
	 * @class myRunnable
	 * @author Jeremy Timperio jmtimper
	 * the code the thread runs to calculate count
	 */
	public class myRunnable implements Runnable {

		//index of thread
		int index;

		/**
		 * constructor
		 * @param i int index of thread
		 */
		public myRunnable(int i) {
			index = i;
		}

		/**
		 * runs circle algorithm to calculate count
		 */
		@Override
		public void run() {

			// look at all the circles.
			for (int i = index; i < cList.size(); i += threads) {
				// Get the range of points that might be inside this
				// circle.
				Circle c = cList.get(i);
				for (int x = c.x - c.r; x <= c.x + c.r; x++) {
					for (int y = c.y - c.r; y <= c.y + c.r; y++) {
						if (inside(c, x, y)) {
							Boolean counted = false;
							// Make sure we haven't already counted this point.
							for (int j = 0; !counted && j < i; j++)
								if (inside(cList.get(j), x, y))
									counted = true;

							// Count the point if it isn't inside an earlier
							// circle.
							if (!counted) {
								count++;
								// System.out.println("count");
							}
						}
					}
				}
			}

		}

	}

	/**
	 * inner class circle to store circle's information
	 * @author Jeremy Timperio jmtimper
	 *
	 */
	public class Circle {
		//x value
		int x;
		//y value
		int y;
		//radius
		int r;

		/**
		 * constructor
		 */
		public Circle() {
			// empty
		}
	}

	/**
	 * Reads circles from inputed file
	 * @param file String 
	 * @throws FileNotFoundException
	 */
	void readCircles(String file) throws FileNotFoundException {

		// Read circle definitions until we can't read any more.
		Circle c;

		//File object
		File sFile = new File(file);
		//Scanner
		Scanner s = new Scanner(sFile);
		
		//reads and creates circle objects line by line
		while (s.hasNextLine()) {
			c = new Circle();
			String lineString = s.nextLine();
			Scanner fline = new Scanner(lineString);
			c.x = fline.nextInt();
			c.y = fline.nextInt();
			c.r = fline.nextInt();

			// Add the circle we just read to the lsit.
			cList.add(c);
		}
	}

	/* Return true if the given point (x, y) is inside the given circle. */
	Boolean inside(Circle c, int x, int y) {
		int dx = x - c.x;
		int dy = y - c.y;
		return dx * dx + dy * dy <= c.r * c.r;
	}

	/**
	 * runs inside.java program
	 * @param args command line arguments
	 * @throws FileNotFoundException no file found
	 * @throws InterruptedException
	 */
	public static void main(String[] args) throws FileNotFoundException, InterruptedException {

		//value length of command line arguments
		if (args.length != 5) {
			throw new IllegalArgumentException("Args exception = " + args.length);
		}

		// Number of points we find inside a circle.
		count = 0;
		
		//initialize arraylist
		cList = new ArrayList<Circle>();
		
		//create instance of inside
		Inside into = new Inside();
		
		//read circles off of text file
		into.readCircles(args[4]);
		
		//initialize thread number
		threads = Integer.parseInt(args[2]);
		
		//creates threads
		for (int i = 0; i < threads; i++) {
			Inside temp = new Inside();
			Thread thread = new Thread(temp.new myRunnable(i));
			thread.start();
			thread.join();
		}

		// Report
		System.out.println("Total: " + count);
	}
}
