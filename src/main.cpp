namespace fancy_status
{
    auto run() -> bool
    {
        if (!run_all_test_cases()) return false;

        print("Hello, ", 42, "\n");
        return true;
    }
}

int main()
{
    return !fancy_status::run();
}

