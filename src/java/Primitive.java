package SE7;

public abstract class Primitive<T> extends Variable {
	private T value;

	public Primitive(String name, T value) {
		super(name);
		this.value = value;
	}

	public T getValue() {
		return this.value;
	}

	public void setValue(T value) {
		this.value = value;
	}
}