# C++/Ruby ABI #

## Describe Interface ##

```ruby
# interfaces.rb
interface Database do
	bool :append_value, (int: value)
	int :get_value, (int: index)
end
```

```C++
# abi_gen.(h? cpp?)
class DatabaseABI
{
	DatabaseABI(Database& db) : m_interface(db) { }
	bool append_value(int value)
	{
		return m_interface.append_value(value);
	}
	int get_value(int index)
	{
		return m_interface.get_value(index);
	}

private:
	Database& m_interface;
};
```