package SE7;

import java.util.Arrays;

import java.io.File;
import java.nio.channels.Pipe;

public class Variant {
	private Object internalVariant;
	private Class[] types;

	public Variant(Class activeClass, Class... clazzes) {
		this.types = new Class[clazzes.length + 1];
		this.types[0] = activeClass;

		for (int i = 1; i < types.length; i++) {
			types[i] = clazzes[i - 1];
		}
	}

	public <T> T get(Class<T> clazz) throws VariantTypeNotFoundException {
		T value = null;

		try {
			value = clazz.cast(this.internalVariant);
		} catch (final ClassCastException classCastException) {
			throw new VariantTypeNotFoundException(clazz);
		}

		return value;
	}

	public <T> T getOr(Class<T> clazz, T returnOnFailure) {
		T value = null;

		try {
			value = clazz.cast(this.internalVariant);
		} catch (final ClassCastException classCastException) {
			return returnOnFailure;
		}

		return value;
	}

	public void set(Object value) throws VariantTypeNotFoundException {
		if (Arrays.stream(this.types).filter(
			cls -> cls.equals(value.getClass())
		).count() == 0) {
			throw new VariantTypeNotFoundException(value.getClass());
		}

		this.internalVariant = value;
	}
}