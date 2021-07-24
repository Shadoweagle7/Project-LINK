package SE7;

public abstract class Variable {
	private String name;

	public Variable(String name) {
		this.name = name;
	}

	public String getName() {
		return this.name;
	}
}