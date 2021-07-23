package SE7;

import java.io.File;

public class Test {
	public static void main(String[] args) {
		if (args.length != 1) {
			System.out.println(
				"Debug mode, 1 argument must be specified, and it must be the toolbox file"
			);

			return;
		}

		try {
			Toolbox t = new Toolbox(new File(args[0]));

			t.get("boing");
		} catch(final Exception e) {
			e.printStackTrace();
		}
	}
}