package SE7;

public class VariantTypeNotFoundException extends Exception {
	public <T> VariantTypeNotFoundException(Class<T> clazzNotFound) {
		super(clazzNotFound.toString() + " not found in Variant");
	}
}