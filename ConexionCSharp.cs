using System.Data.SqlClient;
using System.Data;

class Program{
    // variable de nivel de clase para almacenar el valor cambiado
    public static String ChangedVal;
    //En mi codigo si estan los valores correctos de connectionString
    private static string connectionString = "Data Source=servidor;Initial Catalog=base;User ID=usuario;Password=holaMundo";

    static void Main(string[] args){
        SqlDependency.Start(connectionString);
        StartMonitoring();
        Console.WriteLine(ChangedVal);
        SqlDependency.Stop(connectionString);

    }

    public static void StartMonitoring()
    {
        RegisterForNotification();
    }

    

    private static void RegisterForNotification()
    {
        using (SqlConnection connection = new SqlConnection(connectionString))
        {
            connection.Open();
            using (SqlCommand command = new SqlCommand(
                "SELECT contrasena FROM dbo.Usuarios", connection))
            {
                SqlDependency dependency = new SqlDependency(command);
                dependency.OnChange += new OnChangeEventHandler(OnDependencyChange);
                using (SqlDataReader reader = command.ExecuteReader())
                {
                    //ChangedVal= reader["contrasena"].ToString();
                    //Console.WriteLine("Nueva contraaaaaaaaaa: " + ChangedVal);
                }
            }
        }
    }

    private static String GetNewValFromDB()
    {
        return ChangedVal;
    }

    private static void SaveValToDB(String newVal)
    {
        // aquí debes implementar la lógica para guardar el nuevo valor en la base de datos
        // ...
    }

    private static void OnDependencyChange(object sender, SqlNotificationEventArgs e)
    {
        SqlDependency dependency = sender as SqlDependency;
        dependency.OnChange -= OnDependencyChange;

        if (e.Type == SqlNotificationType.Change)
        {
            // Obtener el nuevo valor
            String newVal = GetNewValFromDB();
            // Almacenar el valor cambiado en la variable de nivel de clase
            ChangedVal = newVal;
            //Almacenar el valor cambiado en la base de datos
            SaveValToDB(newVal);
        }
        // registra una nueva dependencia para detectar cambios adicionales
        RegisterForNotification();
    }

}

