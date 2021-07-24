package SE7;

import java.io.File;
import java.io.FileInputStream;

public class Test {
	public static void main(String[] args) {
		if (args.length != 1) {
			System.out.println(
				"Debug mode, 1 argument must be specified, and it must be the toolbox file"
			);

			return;
		}

		try {
			Toolbox t = new FileToolbox(args[0]);

			Primitive p = new Integer("boing");

			t.get(p);
		} catch(final Exception e) {
			e.printStackTrace();
		}
	}
}